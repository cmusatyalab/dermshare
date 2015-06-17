/*
 * DermShare -- websocket support for mobile camera
 *
 * Copyright (c) 2012-2015 Carnegie Mellon University
 * All rights reserved.
 *
 * This software is distributed under the terms of the Eclipse Public
 * License, Version 1.0 which can be found in the file named LICENSE.
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS SOFTWARE CONSTITUTES
 * RECIPIENT'S ACCEPTANCE OF THIS AGREEMENT
 */

function _ImageSocket(url) {
  var self = this;

  $.extend(this, {
    sock: new WebSocket(url),

    connected: ko.observable(false),
    closed: ko.observable(),

    send_msg: function(type, data) {
      var msg = $.extend({}, data || {}, {
        type: type,
      });
      self.sock.send(JSON.stringify(msg));
    },
  });

  $.extend(self.sock, {
    onopen: function(ev) {
      self.connected(true);
    },

    onclose: function(ev) {
      self.closed(ev.reason || 'Connection closed');
    },
  });

  var ping_timer = setInterval(function() {
    if (self.closed()) {
      clearInterval(ping_timer);
    } else if (self.connected()) {
      self.send_msg('ping');
    }
  }, 30000);
}


function ClientSocket(url, barcode, image) {
  if (!(this instanceof arguments.callee)) {
    throw "Constructor called without new";
  }

  var EXPECT_MSG = 0;
  var EXPECT_BARCODE = 1;
  var EXPECT_IMAGE = 2;

  var self = this;
  var expect = EXPECT_BARCODE;

  _ImageSocket.apply(this, [url]);

  $.extend(this, {
    verifier: ko.observable(),

    kick: function() {
      self.send_msg('kick');
    },
  });

  $.extend(self.sock, {
    onmessage: function(ev) {
      switch (expect) {
      case EXPECT_MSG:
        var msg = JSON.parse(ev.data);
        switch (msg.type) {
        case 'peer':
          self.verifier(msg.verifier);
          break;
        case 'unpeer':
          self.verifier(null);
          break;
        case 'image':
          expect = EXPECT_IMAGE;
          break;
        case 'pong':
          break;
        default:
          self.sock.close(1002, 'Received unexpected message');
          break;
        }
        break;

      case EXPECT_BARCODE:
        expect = EXPECT_MSG;
        barcode(ev.data);
        break;

      case EXPECT_IMAGE:
        expect = EXPECT_MSG;
        image(ev.data);
        self.send_msg('ack');
        break;

      default:
        self.sock.close(1011, 'Socket in unexpected state');
        break;
      }
    },
  });
}


function MobileSocket(url, auth_token) {
  if (!(this instanceof arguments.callee)) {
    throw "Constructor called without new";
  }

  var self = this;

  _ImageSocket.apply(this, [url]);

  $.extend(this, {
    verifier: ko.observable(),
    pending: ko.observable(0),

    send_image: function(blob) {
      self.pending(self.pending() + 1);
      self.send_msg('image');
      self.sock.send(blob);
    },
  });

  var super_onopen = self.sock.onopen;
  $.extend(self.sock, {
    onopen: function() {
      super_onopen();
      self.send_msg('auth', {
        token: auth_token,
      });
    },

    onmessage: function(ev) {
      var msg = JSON.parse(ev.data);
      switch (msg.type) {
      case 'hello':
        self.verifier(msg.verifier);
        break;
      case 'ack':
        if (self.pending()) {
          self.pending(self.pending() - 1);
        }
        break;
      case 'pong':
        break;
      default:
        self.sock.close(1002, 'Received unexpected message');
        break;
      }
    },
  });
}
