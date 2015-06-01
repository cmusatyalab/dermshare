/*
 * DermShare -- utility functions
 *
 * Copyright (c) 2015 Carnegie Mellon University
 * All rights reserved.
 *
 * This software is distributed under the terms of the Eclipse Public
 * License, Version 1.0 which can be found in the file named LICENSE.
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS SOFTWARE CONSTITUTES
 * RECIPIENT'S ACCEPTANCE OF THIS AGREEMENT
 */

// return array grouped into subgroups of @count items each
ko.observableArray.fn.group = function(count) {
  return ko.computed(function() {
    var items = this();
    var ret = [];
    for (var i = 0; i < items.length; i += count) {
      ret.push(items.slice(i, i + count));
    }
    return ret;
  }, this);
};

// button press event handling
ko.bindingHandlers.pressed = {
  init: function(element, valueAccessor) {
    var value = valueAccessor();
    var mouseup = function() { value(false); }

    $(element).on('mousedown.pressed', function() { value(true); });
    $(document).on('mouseup.pressed', mouseup);

    ko.utils.domNodeDisposal.addDisposeCallback(element, function() {
        $(document).off('mouseup.pressed', mouseup);
    });
  },
}
