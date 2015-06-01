/*
 * DermShare -- check for browser support
 *
 * Copyright (c) 2012-2015 Carnegie Mellon University
 * All rights reserved.
 *
 * This software is distributed under the terms of the Eclipse Public
 * License, Version 1.0 which can be found in the file named LICENSE.
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS SOFTWARE CONSTITUTES
 * RECIPIENT'S ACCEPTANCE OF THIS AGREEMENT
 */

(function() {
  var features = [
    'File',
    'HTMLCanvasElement',
    'WebSocket'
  ];
  var url = document.documentElement.getAttribute('data-fallback-url');
  if (!url) {
    return;
  }
  for (var i = 0; i < features.length; i++) {
    if (typeof(window[features[i]]) === 'undefined') {
      document.location = url;
      break;
    }
  }
})();
