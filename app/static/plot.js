/*
 * DermShare -- graphing
 *
 * Copyright (c) 2012-2015 Carnegie Mellon University
 * All rights reserved.
 *
 * This software is distributed under the terms of the Eclipse Public
 * License, Version 1.0 which can be found in the file named LICENSE.
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS SOFTWARE CONSTITUTES
 * RECIPIENT'S ACCEPTANCE OF THIS AGREEMENT
 */

function HistogramPlot(args) {
  function arg_array(arr, valueAccessor) {
    if (typeof(arr) === 'function') {
      arr = arr(valueAccessor);
    }
    if (!arr) {
      return arr;
    }
    // Array.reverse() reverses in-place
    arr = arr.slice(0);
    arr.reverse();
    return arr;
  }
  $.extend(this, {
    init: function(element, valueAccessor, allBindings) {
      if (!element.id) {
        element.id = 'histogram-' + Math.round(Math.random() * 100000000);
      }
      var histogram = $.jqplot(element.id, [[0]], {
        title: {
          text: args.title,
          fontFamily: $('body').css('font-family'),
          fontSize: $('body').css('font-size'),
        },
        axesDefaults: {
          tickOptions: {
            fontFamily: $('body').css('font-family'),
            fontSize: '11px',
          },
        },
        axes: {
          xaxis: {
            min: 0,
            max: 100,
            tickInterval: 20,
            tickOptions: {
              formatString: '%.0f%%',
            },
          },
          yaxis: {
            renderer: $.jqplot.CategoryAxisRenderer,
            ticks: arg_array(args.buckets, valueAccessor()),
            tickOptions: {
              showGridline: false,
              showMark: false,
            }
          },
        },
        grid: {
          background: '#f9f9f9',
        },
        seriesDefaults: {
          renderer: $.jqplot.BarRenderer,
          rendererOptions: {
            barDirection: 'horizontal',
            barMargin: 5,
            shadowDepth: 3,
            varyBarColor: true,
          },
        },
        highlighter: {
          show: true,
          showMarker: false,
          tooltipAxes: 'x',
          tooltipLocation: 'ne',
        },
      });
      $(element).data('_histogram', histogram);
      // redraw plot when any of the monitored observables changes
      $.each(allBindings.get('monitor') || [], function(i, monitor) {
        monitor.subscribe(function() {
          if ($(element).is(':visible')) {
            histogram.replot();
          }
        });
      });
    },

    update: function(element, valueAccessor) {
      var histogram = $(element).data('_histogram');
      var points = arg_array(args.data, valueAccessor());
      var data = [];

      for (var i = 0; i < points.length; i++) {
        data.push([points[i], i + 1]);
      }
      histogram.series[0].data = data;
      histogram.replot();
    },
  });
}


ko.bindingHandlers.diagnosisHistogram = new HistogramPlot({
  title: 'Diagnosis Histogram',
  buckets: function(value) {
    var diagnoses = ko.unwrap(value);
    return diagnoses.names;
  },
  data: function(value) {
    var diagnoses = ko.unwrap(value);
    var data = [];
    $.each(diagnoses.names, function(i, d) {
      data.push(100 * diagnoses.histogram[d]() / diagnoses.total());
    });
    return data;
  },
});
