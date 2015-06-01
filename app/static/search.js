/*
 * DermShare -- Diamond search handling
 *
 * Copyright (c) 2012-2015 Carnegie Mellon University
 * All rights reserved.
 *
 * This software is distributed under the terms of the Eclipse Public
 * License, Version 1.0 which can be found in the file named LICENSE.
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS SOFTWARE CONSTITUTES
 * RECIPIENT'S ACCEPTANCE OF THIS AGREEMENT
 */

// scopeserver modal dialog
// registers global message handler!
// argument: observable indicating whether the scope window is showing
// scopeCookie: observable storing the current scope
// scopeServer: scope server URL
ko.bindingHandlers.scopeModal = {
  init: function(element, valueAccessor, allBindings) {
    // configure element; create iframe inside it
    $(element).addClass('modal hide fade');
    var frame = $('<iframe></iframe>').addClass('modal-body spinner').
        attr('sandbox', 'allow-same-origin allow-forms allow-scripts');
    $(element).empty().append(frame);
    var scope_window = frame.prop('contentWindow');

    // accept scope cookie message from iframe
    $(window).on('message', function(ev) {
      if (ev.originalEvent.source !== scope_window) {
        return;
      }
      var data = $.parseJSON(ev.originalEvent.data);
      if (!data.cancel) {
        // set new cookie
        allBindings.get('scopeCookie')(data);
      }
      // close modal
      valueAccessor()(false);
    });

    // reset iframe to blank when modal is hidden
    $(element).on('hidden', function() {
      scope_window.location = 'about:blank';
    });
  },

  update: function(element, valueAccessor, allBindings) {
    if (ko.unwrap(valueAccessor())) {
      // open scope modal
      $(element).find('iframe').prop('contentWindow').location =
          allBindings.get('scopeServer');
      $(element).modal('show');
    } else {
      // close it
      $(element).modal('hide');
    }
  },
};


// search info box
(function() {
  function update_widths() {
    $('.search-info-monitored-width').width(function() {
      return $(this).prev().width();
    });
  }

  // argument: arguments for Bootstrap affix constructor
  ko.bindingHandlers.searchInfo = {
    init: function(element, valueAccessor) {
      // Add element, before this one, with the same classes, no visibility,
      // and no height.  Use it to detect the width the info well would
      // have had so we can update the width of the info well while it has
      // position: fixed.
      $(element).before($('<div></div>')
          .addClass($(element).prop('class'))
          .height(0)
          .css('min-height', '0')
          .css('margin-top', '0')
          .css('margin-bottom', '0')
          .css('padding-top', '0')
          .css('padding-bottom', '0')
          .css('border-top-width', '0')
          .css('border-bottom-width', '0')
          .css('visibility', 'hidden'));
      $(element).addClass('search-info-monitored-width')
          .affix(valueAccessor());
      update_widths();
    },
  };

  // Update info box widths whenever the window is resized
  $(window).resize(update_widths);
}());


// hover handler for result thumbnail
// argument: Result
ko.bindingHandlers.hoverThumbnail = {
  init: function(element, valueAccessor) {
    var result = valueAccessor();
    $(element).on('mouseover', function() {
      result.search.highlightedResult(result);
    }).on('mouseout', function() {
      result.search.highlightedResult(null);
    });
  },
};


function Diagnosis(bucket, text) {
  if (!(this instanceof arguments.callee)) {
    throw "Constructor called without new";
  }

  $.extend(this, {
    bucket: bucket,
    text: text,
  });
}


function Diagnoses() {
  if (!(this instanceof arguments.callee)) {
    throw "Constructor called without new";
  }

  var self = this;

  var names = {
    bcc: 'Basal cell carcinoma',
    bennev: 'Benign nevus',
    blue: 'Blue nevus',
    df: 'Dermatofibroma',
    hgdn: 'High-grade dysplastic nevus',
    invmm: 'Invasive melanoma',
    lentigo: 'Lentigo',
    lgdn: 'Low-grade dysplastic nevus',
    lplk: 'Lichen planus-like keratosis',
    mminv: 'Invasive melanoma',
    mmis: 'Melanoma in situ',
    scc: 'Squamous-cell carcinoma',
    sk: 'Seborrheic keratosis',
    spitzatypical: 'Atypical Spitz nevus',
    spitznevus: 'Spitz nevus, no atypical feat.',
    vascular: 'Benign vascular lesion',
  }
  var buckets = [
    ['Melanoma', 'invmm', 'mminv', 'mmis'],
    ['Non-melanoma skin cancer', 'bcc', 'scc'],
    ['Benign melanocytic lesion', 'bennev', 'blue', 'lentigo', 'lgdn',
        'spitznevus'],
    ['Other benign lesion', 'df', 'lplk', 'sk', 'vascular'],
    ['Borderline lesions', 'hgdn', 'spitzatypical'],
  ];
  var other = 'Other';
  var unknown = 'Unknown';

  this.names = [];
  this.histogram = {};
  function add(name) {
    self.names.push(name);
    self.histogram[name] = ko.observable(0);
  }
  for (var i = 0; i < buckets.length; i++) {
    add(buckets[i][0]);
  }
  add(other);
  add(unknown);

  $.extend(this, {
    total: ko.observable(0),

    get_for_attrs: function(attrs) {
      // get diagnosis string
      var diag = attrs['dermshare.diagnosis'];
      if (!diag) {
        // no diagnosis
        return new Diagnosis(unknown, 'Unknown');
      }
      diag = diag.data;
      var tag = diag.toLowerCase();

      // walk possible histogram buckets
      for (var i = 0; i < buckets.length; i++) {
        var bucket = buckets[i];
        // walk tags for this bucket
        for (var j = 1; j < bucket.length; j++) {
          if (bucket[j] === tag) {
            return new Diagnosis(bucket[0], names[tag] || 'Unknown');
          }
        }
      }

      // unrecognized
      return new Diagnosis(other, diag);
    },

    record: function(diagnosis) {
      var observable = self.histogram[diagnosis.bucket];
      observable(observable() + 1);
      self.total(self.total() + 1);
    },
  });
}


function Search(options, id, params, example, exampleFile, segmImage) {
  if (!(this instanceof arguments.callee)) {
    throw "Constructor called without new";
  }

  var self = this;

  this.arguments = {};
  $.each(params, function(i, d) {
    var value = d.argument;
    if (d.type === 'scope') {
      value = value().cookie;
    } else if (d.type === 'number' || d.type === 'range') {
      value = value.parsed();
    } else {
      console.log('Invalid data type ' + d.type);
      return;
    }
    self.arguments[d.name] = value;
  });

  $.extend(this, {
    id: id,
    name: 'Search ' + id,
    example: example,
    options: options,
    search: null,
    exampleAttrs: null,
    error: ko.observable(),
    results: ko.observableArray(),
    highlightedResult: ko.observable(null),
    diagnoses: new Diagnoses(),

    started: ko.observable(false),
    progress: ko.observable(),
    complete: ko.observable(false),
    closed: ko.observable(false),
    running: ko.observable(true),
    visible: ko.observable(false),

    run: function() {
      var form = new FormData();
      form.append('csrf_token', self.options.csrf_token);
      form.append('example', exampleFile);
      form.append('segmentation', segmImage);
      $.each(self.arguments, function(k, v) {
        form.append(k, v);
      });
      $.ajax({
        // request
        type: 'POST',
        url: self.options.search_url,
        data: form,
        contentType: false,
        processData: false,

        // reply
        dataType: 'json',
        success: function(data, textStatus, jqXHR) {
          self.started(true);
          self.exampleAttrs = data.example_attrs;
          self.search = new JSONBlasterSocket(data.socket_url, data.search_key);
          self.search.on('result', function(data) {
            var result = new Result(self, data, self.diagnoses);
            self.results.push(result);
          });
          self.search.on('statistics', function(data) {
            if (data.objs_total) {
              self.progress(data.objs_processed / data.objs_total);
            } else {
              self.progress(0);
            }
          });
          self.search.on('search_complete', function(data) {
            self.complete(true);
          });
          self.search.on('error', function(data) {
            self.error(data.message);
          });
          self.search.onclose(function(data) {
            if (!data.wasClean) {
              self.error(data.reason);
            } else if (!self.complete()) {
              self.error("The search unexpectedly terminated.");
            }
            self.closed(true);
          });
        },
        error: function(jqXHR, textStatus, errorThrown) {
          self.error(jqXHR.responseText);
          self.closed(true);
        }
      });
    },

    close: function() {
      if (self.search) {
        self.search.close();
      }
    },

    clearError: function() {
      self.error(null);
    },
  });

  // post-process results and clear running observable when search stops
  var searchDone = ko.computed(function() {
    return self.complete() || self.closed();
  });
  var wasDone = false;
  var preloading = 0;
  searchDone.subscribe(function(done) {
    if (done && !wasDone) {
      // sort results
      var key = '_filter.distance_score';
      self.results.sort(function(a, b) {
        var aa = parseFloat(a.attrs[key].data);
        var bb = parseFloat(b.attrs[key].data);
        if (aa < bb) {
          return -1;
        } else if (aa > bb) {
          return 1;
        } else if (a.attrs._ObjectID.data < b.attrs._ObjectID.data) {
          return -1;
        } else {
          return 1;
        }
      });

      // limit to configured result count
      if (self.results().length > self.arguments.num_results) {
        self.results.splice(self.arguments.num_results,
            self.results().length - self.arguments.num_results);
      }

      $.each(self.results(), function(i, d) {
        // set result id
        d.id = i + 1;

        // populate diagnoses
        // Add directly to diagnoses.  Having diagnoses subscribe to
        // self.results would cause O(n^2) work
        self.diagnoses.record(d.diagnosis);

        // preload first 50 thumbnails
        if (i < 50) {
          var image = new Image();
          $(image).load(function() {
            preloading--;
            if (preloading === 0) {
              // all preloaded; show results
              self.running(false);
            }
          });
          preloading++;
          image.src = d.thumbnail_url;
        }
      });

      if (self.results().length === 0) {
        // no images to preload, so complete now
        self.running(false);
      }
    }
    wasDone = done;
  });

  var viewedResults = ko.observable(false);
  this.newResults = ko.computed(function() {
    if (!self.running()) {
      if (self.visible()) {
        viewedResults(true);
      }
      return !viewedResults();
    } else {
      return false;
    }
  });
}


function Result(search, attrs, diagnoses) {
  if (!(this instanceof arguments.callee)) {
    throw "Constructor called without new";
  }

  $.extend(this, {
    id: null,  // set after results are sorted
    search: search,
    attrs: attrs,
    thumbnail_url: attrs['thumbnail.jpeg'].image_url,
    large_thumbnail_url: attrs['large-thumbnail.jpeg'].image_url,
    diagnosis: diagnoses.get_for_attrs(attrs),
  });
}


function ResultTab(result) {
  if (!(this instanceof arguments.callee)) {
    throw "Constructor called without new";
  }

  var self = this;
  var id = result.search.id + '-' + result.id;

  $.extend(this, {
    result: result,
    id: id,
    name: 'Result ' + id,

    error: ko.observable(null),

    image: new Image(),
    imageLoading: ko.observable(false),

    description: ko.observable(''),
    descriptionLoading: ko.observable(false),

    clearError: function() {
      self.error(null);
    },

    loadImage: function() {
      if (self.image.src) {
        return;
      }
      self.imageLoading(true);
      $(self.image).load(function() {
        self.imageLoading(false);
      });
      self.image.src = self.result.attrs[''].image_url;
    },

    loadDescription: function() {
      var url = self.result.attrs['dermshare.pathology-report-url'];
      if (url) {
        url = url.data;
      }
      if (self.description() || self.descriptionLoading() || !url) {
        return;
      }
      self.descriptionLoading(true);
      $.ajax({
        url: url,
        success: function(data, textStatus, jqXHR) {
          self.description(data);
        },
        error: function(jqXHR, textStatus, errorThrown) {
          self.error(jqXHR.responseText);
        },
        complete: function(jqXHR, textStatus) {
          self.descriptionLoading(false);
        },
      });
    },

    getFeatures: function() {
      // return array of objects with attributes: feature, example, result
      var prefix = 'gemini.gemini.';
      var ret = [];
      function format(value) {
        return new Number(value).toFixed(3);
      }
      $.each(result.search.exampleAttrs, function(k, example_v) {
        if (k.indexOf(prefix) !== 0) {
          return;
        }
        ret.push({
          feature: k.substr(prefix.length),
          example: format(example_v.data),
          result: format(result.attrs[k].data),
        });
      });
      ret.sort(function(a, b) {
        if (a.feature < b.feature) {
          return -1;
        } else if (a.feature > b.feature) {
          return 1;
        } else {
          return 0;
        }
      });
      return ret;
    }
  });
}
