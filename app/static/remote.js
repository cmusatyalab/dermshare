/*
 * DermShare -- remote camera application
 *
 * Copyright (c) 2012-2015 Carnegie Mellon University
 * All rights reserved.
 *
 * This software is distributed under the terms of the Eclipse Public
 * License, Version 1.0 which can be found in the file named LICENSE.
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS SOFTWARE CONSTITUTES
 * RECIPIENT'S ACCEPTANCE OF THIS AGREEMENT
 */

function RemoteApp(ws_url, auth_token) {
  if (!(this instanceof arguments.callee)) {
    throw "Constructor called without new";
  }

  var self = this;

  $.extend(this, {
    image: ko.computed({
      read: function() {return null},
      write: function(file) {
        self.sock.send_image(file);
      },
    }),
  });

  this.sock = new MobileSocket(ws_url, auth_token);
}
