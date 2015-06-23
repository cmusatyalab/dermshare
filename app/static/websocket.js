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

function _ImageSocket(args) {
  var self = this;

  $.extend(this, {
    connected: ko.observable(false),
    closed: ko.observable(null),

    send_msg: function(type, data) {
      var msg = $.extend({}, data || {}, {
        type: type,
      });
      self.sock.send(JSON.stringify(msg));
    },
  });

  function connect() {
    if (args.url) {
      self.sock = new WebSocket(args.url);
    } else {
      self.sock = {};
      self.closed('WebSockets not configured');
    }

    $.extend(self.sock, {
      onopen: function(ev) {
        self.connected(true);
        if (args.open) {
          args.open(ev);
        }
      },

      onmessage: function(ev) {
        args.message(ev);
      },

      onclose: function(ev) {
        if (args.reopen && args.reopen(ev)) {
          self.connected(false);
          setTimeout(connect, 1000);
        } else {
          self.closed(ev.reason || 'Connection closed');
          if (args.close) {
            args.close(ev);
          }
        }
      },
    });
  }
  connect();

  var ping_timer = setInterval(function() {
    if (self.closed()) {
      clearInterval(ping_timer);
    } else if (self.connected()) {
      self.send_msg('ping');
    }
  }, 30000);
}


function ClientSocket(url, barcode, image, imageSaved) {
  if (!(this instanceof arguments.callee)) {
    throw "Constructor called without new";
  }

  var EXPECT_MSG = 0;
  var EXPECT_BARCODE = 1;
  var EXPECT_IMAGE = 2;

  var self = this;
  var expect;
  var image_type;

  function init_socket() {
    expect = EXPECT_BARCODE;
    self.verifier(null);
  };

  _ImageSocket.apply(this, [{
    url: url,

    message: function(ev) {
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
          image_type = msg.content_type;
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
        barcode(new Blob([ev.data], {
          // server always sends PNG
          type: 'image/png',
        }));
        break;

      case EXPECT_IMAGE:
        expect = EXPECT_MSG;
        image(new Blob([ev.data], {
          type: image_type,
        }));
        imageSaved(false);
        self.send_msg('ack');
        break;

      default:
        self.sock.close(1011, 'Socket in unexpected state');
        break;
      }
    },

    reopen: function() {
      init_socket();
      return true;
    },
  }]);

  $.extend(this, {
    verifier: ko.observable(),

    kick: function() {
      self.send_msg('kick');
    },
  });

  init_socket();
}


function MobileSocket(url, auth_token) {
  if (!(this instanceof arguments.callee)) {
    throw "Constructor called without new";
  }

  var self = this;

  _ImageSocket.apply(this, [{
    url: url,

    open: function() {
      self.send_msg('auth', {
        token: auth_token,
      });
    },

    message: function(ev) {
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
  }]);

  $.extend(this, {
    verifier: ko.observable(),
    pending: ko.observable(0),

    send_image: function(blob) {
      self.pending(self.pending() + 1);
      self.send_msg('image', {
        content_type: blob.type,
      });
      self.sock.send(blob);
    },
  });
}
