/*
 * DermShare -- top-level application
 *
 * Copyright (c) 2012-2015 Carnegie Mellon University
 * All rights reserved.
 *
 * This software is distributed under the terms of the Eclipse Public
 * License, Version 1.0 which can be found in the file named LICENSE.
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS SOFTWARE CONSTITUTES
 * RECIPIENT'S ACCEPTANCE OF THIS AGREEMENT
 */

ko.extenders.searchField = function(target, param) {
  target.parsed = ko.observable();  // parsed value of target
  target.active = ko.observable();  // whether user has enabled the target
  target.valid = ko.observable();  // whether the target passes validation

  target.active(!!param.mandatory);

  if (param.type === 'number') {
    target(param.defaul * param.scale);

    target.parsed = ko.computed(function() {
      var v = this();
      if (!this.active()) {
        return null;
      } else if (v === '') {
        return NaN;
      } else {
        // parseFloat ignores trailing garbage
        return (v - 0) / param.scale;
      }
    }, target);

    function update(value) {
      value = target.parsed();
      target.valid(!isNaN(value) && value >= param.min &&
          value <= param.max);
    }
    update(target());
    target.subscribe(update);
    target.active.subscribe(update);

  } else if (param.type === 'scope') {
    target.parsed = target;
    target.valid = ko.computed(function() {
      return !!(this() && this().hasOwnProperty('description') &&
          this().hasOwnProperty('cookie'));
    }, target);

  } else if (param.type === 'range') {
    // create two nested observables
    target = ko.computed(function() {
      // ranges are a composite type, so there is no meaningful raw value
      return null;
    });
    $.each(['low', 'high'], function(i, d) {
      param[d].type = 'number';
      param[d].argument = ko.observable('').extend({searchField: param[d]});
    });

    target.parsed = ko.computed(function() {
      return [param.low.argument.parsed(), param.high.argument.parsed()];
    });
    var active = ko.observable(!!param.mandatory);
    target.active = ko.computed({
      read: function() {
        return active();
      },
      write: function(value) {
        param.low.argument.active(value);
        param.high.argument.active(value);
        active(value);
      },
    });
    target.valid = ko.computed(function() {
      return param.low.argument.valid() && param.high.argument.valid();
    });

  } else {
    console.log('Unknown validation type ' + param.type);
    target.parsed('');
    target.valid(false);
  }

  return target;
};


// track current tab
// argument: the list of removable tabs
// currentTab: observable giving the current tab object
// <a> elements must have data-tab-id attributes corresponding to a tab
// object's "id" property
ko.bindingHandlers.tabs = {
  init: function(element, valueAccessor, allBindings) {
    $(element).on('shown', 'a[data-toggle="tab"]', function(ev) {
      var value = valueAccessor();
      var cur_tab = allBindings.get('currentTab');
      var cur_tab_value = cur_tab();
      if (cur_tab_value && cur_tab_value.visible) {
        cur_tab_value.visible(false);
      }
      for (var i = 0; i < value.length; i++) {
        if ($(ev.target).data('tab-id') + '' === value[i].id + '') {
          cur_tab(value[i]);
          if (value[i].visible) {
            value[i].visible(true);
          }
          return;
        }
      }
      cur_tab($(ev.target).attr('href'));
    });
  },

  update: function(element, valueAccessor, allBindings) {
    var cur_tab = allBindings.get('currentTab')();
    if (cur_tab && typeof(cur_tab.id) !== 'undefined') {
      $(element).find('a[href="#tab-' + cur_tab.id + '"]').tab('show');
    } else {
      $(element).find('a[href="' + cur_tab + '"]').tab('show');
    }
  },
};



function App(options) {
  if (!(this instanceof arguments.callee)) {
    throw "Constructor called without new";
  }

  var self = this;

  this.parameters = [
    {name: 'scope', displayName: 'Scope',
        type: 'scope', mandatory: true},
    {name: 'num_results', displayName: 'Result Count',
        type: 'number', mandatory: true,
        defaul: 12, min: 1, max: 1000, scale: 1},
  ];
  this.arguments = {};
  $.each(this.parameters, function(i, p) {
    p.argument = ko.observable('').extend({searchField: p});
    self.arguments[p.name] = p.argument;
  });

  var barcodeData = ko.observable();
  var exampleData = ko.observable();
  var exampleFile;

  $.extend(this, {
    showingScope: ko.observable(false),

    tabs: ko.observableArray(),
    currentTab: ko.observable(),
    nextSearch: ko.observable(1),

    example: ko.computed({
      read: exampleData,
      write: function(file) {
        self.clearError();
        if (!file) {
          exampleFile = null;
          exampleData(null);
          self.exampleFilename(null);
          if (self.exampleObjectURL()) {
            URL.revokeObjectURL(self.exampleObjectURL());
            self.exampleObjectURL(null);
          }
          self.autoSegm(null);
          self.autoSegmRunning(false);
          return;
        }

        var reader = new FileReader();
        reader.onload = function() {
          exampleFile = file;
          exampleData(reader.result);
          if (self.exampleObjectURL()) {
            URL.revokeObjectURL(self.exampleObjectURL());
          }
          self.exampleObjectURL(URL.createObjectURL(file));
          self.autoSegm(null);
          self.autoSegmRunning(true);

          switch (file.type) {
          case 'image/jpeg':
            self.exampleFilename('example.jpg');
            break;
          case 'image/png':
            self.exampleFilename('example.png');
            break;
          default:
            self.exampleFilename('example');
            break;
          }

          var form = new FormData();
          form.append('csrf_token', options.csrf_token);
          form.append('example', file);
          $.ajax({
            url: options.segment_url,
            type: 'POST',
            // request data
            data: form,
            contentType: false,
            processData: false,
            // response data
            dataType: 'json',
            success: function(data, textStatus, jqXHR) {
              // most recent example wins
              if (exampleData() === reader.result) {
                self.autoSegm(data.url);
                self.autoSegmRunning(false);
              }
            },
            error: function(jqXHR, textStatus, errorThrown) {
              // most recent example wins
              if (exampleData() === reader.result) {
                self.error(jqXHR.responseText);
                self.autoSegmRunning(false);
              }
            },
          });
        };
        reader.readAsDataURL(file);
      },
    }, this),
    exampleFilename: ko.observable(),
    exampleObjectURL: ko.observable(),
    exampleSaved: ko.observable(),
    autoSegmRunning: ko.observable(false),
    autoSegm: ko.observable(null),

    barcode: ko.computed({
      read: barcodeData,
      write: function(file) {
        if (!file) {
          barcodeData(null);
          return;
        }
        var reader = new FileReader();
        reader.onload = function() {
          barcodeData(reader.result);
        };
        reader.readAsDataURL(file);
      },
    }, this),

    error: ko.observable(null),

    createSearch: function() {
      var id = self.nextSearch();
      self.nextSearch(id + 1);
      var search = new Search(options, id, self.parameters,
          self.example(), exampleFile, self.segmenter.segmGetter()());
      self.tabs.push(search);
      self.currentTab(search);
      search.run();
    },

    createResultTab: function(result) {
      var tabs = self.tabs();
      for (var i = 0; i < tabs.length; i++) {
        if (tabs[i].result === result) {
          self.currentTab(tabs[i]);
          return;
        }
      }
      var tab = new ResultTab(result);
      self.tabs.push(tab);
      tab.loadDescription();
      self.currentTab(tab);
    },

    destroyTab: function(obj) {
      if (obj.close) {
        obj.close();
      }
      self.tabs.remove(obj);
      if (self.currentTab() === obj) {
        if (obj.constructor === ResultTab &&
            self.tabs.indexOf(obj.result.search) != -1) {
          self.currentTab(obj.result.search);
          return;
        }
        self.currentTab('#examine');
      }
    },

    clearError: function() {
      self.error(null);
    },

    setExampleSaved: function() {
      // don't mark the example saved if the browser can't do it
      if (self.haveDownloadAttribute) {
        self.exampleSaved(true);
      }
      // don't prevent default
      return true;
    }
  });

  this.sock = new ClientSocket(options.ws_url, this.barcode, this.example,
      this.exampleSaved);

  this.haveSegmentation = ko.computed(function() {
    return (this.example() && !this.autoSegmRunning() &&
        !this.segmenter.autoSegmFetching());
  }, this);

  this.segmenter = new SegmenterModel(this.haveSegmentation, this.example,
      this.autoSegm);

  this.argumentsValid = ko.computed(function() {
    if (!this.haveSegmentation()) {
      return false;
    }
    for (var i = 0; i < this.parameters.length; i++) {
      var o = this.parameters[i].argument;
      if (o.active() && !o.valid()) {
        return false;
      }
    }
    return true;
  }, this);

  this.haveDownloadAttribute = (typeof $('<a>')[0].download !== 'undefined');
}
