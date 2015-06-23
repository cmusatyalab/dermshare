/*
 * DermShare -- importing example images
 *
 * Copyright (c) 2012-2015 Carnegie Mellon University
 * All rights reserved.
 *
 * This software is distributed under the terms of the Eclipse Public
 * License, Version 1.0 which can be found in the file named LICENSE.
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS SOFTWARE CONSTITUTES
 * RECIPIENT'S ACCEPTANCE OF THIS AGREEMENT
 */

ko.bindingHandlers.disabledDropZone = {
  // Prevent accidental drag-and-drop from replacing document
  init: function(element) {
    $(element).on({
      'dragover': function(ev) {
        ev.stopPropagation();
        ev.preventDefault();
        ev.originalEvent.dataTransfer.dropEffect = 'none';
      },
      'drop': function(ev) {
        // Sometimes dragover doesn't fire on a particular element.
        // In this case we can't affect the mouse cursor, but we can
        // still reject the drop.
        ev.stopPropagation();
        ev.preventDefault();
      },
    });
  },
};


ko.bindingHandlers.exampleDropZone = {
  // example drag-and-drop
  init: function(element, valueAccessor, allBindings) {
    $(element).on({
      'dragenter': function(ev) {
        // we are a drop target; prevent drops from propagating
        ev.stopPropagation();
        ev.preventDefault();
      },
      'dragover': function(ev) {
        ev.stopPropagation();
        ev.preventDefault();
        var xfer = ev.originalEvent.dataTransfer;
        var accept;
        if (xfer.items) {
          // New DataTransfer interface (e.g. Chrome)
          accept = xfer.items.length === 1 &&
              xfer.items[0].kind === 'file' &&
              xfer.items[0].type.indexOf('image/') === 0;
        } else if (typeof(xfer.mozItemCount) !== 'undefined') {
          // Firefox (through at least version 16)
          accept = xfer.mozItemCount === 1;
        } else {
          // The old DataTransfer interface doesn't tell us anything useful
          accept = true;
        }
        if (accept) {
          // accept drop
          xfer.dropEffect = 'copy';
          $(element).addClass('drop-accepted');
        } else {
          // reject drop
          xfer.dropEffect = 'none';
        }
      },
      'dragleave': function(ev) {
        // stop accepting drop
        $(element).removeClass('drop-accepted');
      },
      'drop': function(ev) {
        // process drop
        ev.stopPropagation();
        ev.preventDefault();
        $(element).removeClass('drop-accepted');
        // We provided user feedback that we would accept the drop, even if
        // we couldn't check whether the contents were valid.  Do our best
        // to find an acceptable file.
        var files = ev.originalEvent.dataTransfer.files;
        var accepted_file = null;
        for (var i = 0; i < files.length; i++) {
          if (files[i].type.indexOf('image/') === 0) {
            accepted_file = files[i];
            break;
          }
        }
        valueAccessor()(accepted_file);
        allBindings.get('exampleDropZoneSaved')(true);
      },
    });
  },
};


ko.bindingHandlers.openFileWidget = {
  // open dialog for examples
  init: function(element, valueAccessor, allBindings) {
    var form = $('<form class="hide">').insertAfter(element);
    var input = $('<input type="file" accept="image/*">').appendTo(form);
    if (allBindings.get('openFileCapture')) {
      input.attr('capture', true);
    }
    input.on('change', function(ev) {
      valueAccessor()(this.files[0]);
      var saved = allBindings.get('openFileSaved');
      if (saved) {
        saved(true);
      }
      form[0].reset();
    });
    $(element).click(function(ev) {
      ev.preventDefault();
      input.click();
      this.blur();
    });
  },
};
