#!/usr/bin/env python
#
# DermShare -- WebSocket server for mobile devices
#
# Copyright (c) 2015 Carnegie Mellon University
# All rights reserved.
#
# This software is distributed under the terms of the Eclipse Public
# License, Version 1.0 which can be found in the file named LICENSE.
# ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS SOFTWARE CONSTITUTES
# RECIPIENT'S ACCEPTANCE OF THIS AGREEMENT
#

import base64
from cStringIO import StringIO
from geventwebsocket import WebSocketServer, WebSocketApplication, Resource
import json
import os
import qrcode
import random
import sys
from urlparse import urljoin
import yaml

try:
    from collections import OrderedDict
except ImportError:
    # Python 2.6
    from ordereddict import OrderedDict

config = {
    # Address to listen on (ignored under gunicorn)
    'listen': '127.0.0.1',
    # URL of mobile site (defaulted if not specified)
    'mobile_url': None,
    # Permitted HTTP origins
    'origins': [],
    # Port to listen on (ignored under gunicorn)
    'port': 5003,
}

class _WSError(Exception):
    def __init__(self, code, msg):
        self.code = code
        self.msg = msg


class _ImageRelayConnection(WebSocketApplication):
    def __init__(self, *args, **kwargs):
        WebSocketApplication.__init__(self, *args, **kwargs)
        self.peer = None

    def on_open(self):
        if self.ws.origin not in config['origins']:
            print 'Origin prohibited: {0}'.format(self.ws.origin)
            self.close(1008, 'Origin prohibited')
            return
        self._on_open()

    def on_message(self, msg):
        if msg is None:
            return
        try:
            self._on_message(msg)
        except (TypeError, KeyError, ValueError):
            self.close(1002, 'Protocol error')
        except _WSError, e:
            self.close(e.code, e.msg)

    def on_close(self, _reason):
        if self.peer:
            self.peer.unpeer()
        self._on_close()

    def send_msg(self, type, **kwargs):
        msg = {
            'type': type,
        }
        msg.update(kwargs)
        self.ws.send(json.dumps(msg))

    def send_blob(self, data):
        self.ws.send(data, binary=True)

    def unpeer(self):
        pass

    def close(self, code, msg):
        self.ws.close(code, msg)

    def _on_open(self):
        pass

    def _on_message(self, msg):
        raise ValueError('Invalid message')

    def _on_close(self):
        pass


class ImageClientConnection(_ImageRelayConnection):
    _connections = {}  # token -> ImageClientConnection

    def __init__(self, *args, **kwargs):
        _ImageRelayConnection.__init__(self, *args, **kwargs)
        self.base_url = (config['mobile_url'] or
                urljoin(config['origins'][0], '/remote/'))
        self.token = base64.urlsafe_b64encode(os.urandom(24))
        self.verifier = None

    @classmethod
    def peer(cls, token, peer):
        conn = cls._connections.pop(token, None)
        if conn is None:
            return None
        conn.peer = peer
        conn.verifier = '{0:04}'.format(random.randint(0, 9999))
        conn.send_msg('peer', verifier=conn.verifier)
        return conn

    def _allow_peering(self):
        self._connections[self.token] = self

    def unpeer(self):
        self.peer = None
        self._allow_peering()
        self.send_msg('unpeer')

    def _on_open(self):
        url = urljoin(self.base_url, self.token)
        barcode = qrcode.make(url, box_size=4, border=0,
                error_correction=qrcode.constants.ERROR_CORRECT_L)
        buf = StringIO()
        barcode.save(buf, 'png')
        self.send_blob(buf.getvalue())
        self._allow_peering()

    def _on_message(self, msg):
        msg = json.loads(msg)
        if msg['type'] == 'ack':
            if self.peer:
                self.peer.send_msg('ack')
        elif msg['type'] == 'kick':
            if self.peer:
                self.peer.close(1000, 'Connection terminated by client')
        elif msg['type'] == 'ping':
            self.send_msg('pong')
        else:
            raise ValueError('Invalid message')

    def _on_close(self):
        self._connections.pop(self.token, None)


class ImageMobileConnection(_ImageRelayConnection):
    STATE_AUTHENTICATING = 0
    STATE_RUNNING = 1
    STATE_SENDING_IMAGE = 2

    def __init__(self, *args, **kwargs):
        _ImageRelayConnection.__init__(self, *args, **kwargs)
        self.state = self.STATE_AUTHENTICATING
        self.image_type = None

    def unpeer(self):
        self.peer = None
        self.close(1000, 'Session closed')

    def _on_message(self, msg):
        if self.state == self.STATE_AUTHENTICATING:
            msg = json.loads(msg)
            if msg['type'] != 'auth':
                raise _WSError(1002, 'Authentication required')
            self.peer = ImageClientConnection.peer(msg['token'], self)
            if not self.peer:
                raise _WSError(1008, 'Authentication failed')
            self.state = self.STATE_RUNNING
            self.send_msg('hello', verifier=self.peer.verifier)
        elif self.state == self.STATE_RUNNING:
            msg = json.loads(msg)
            if msg['type'] == 'image':
                self.state = self.STATE_SENDING_IMAGE
                self.image_type = msg['content_type']
            elif msg['type'] == 'ping':
                self.send_msg('pong')
            else:
                raise ValueError('Invalid message')
        elif self.state == self.STATE_SENDING_IMAGE:
            if self.peer:
                self.peer.send_msg('image', content_type=self.image_type)
                self.peer.send_blob(msg)
            self.state = self.STATE_RUNNING
        else:
            raise ValueError('Invalid state')


def _load_settings(path):
    if path:
        with open(path) as fh:
            config.update(yaml.safe_load(fh))


_load_settings(os.environ.get('DERMSHARE_WS_SETTINGS'))


resources = Resource(OrderedDict((
    ('/ws/client', ImageClientConnection),
    ('/ws/mobile', ImageMobileConnection),
)))


if __name__ == '__main__':
    try:
        _load_settings(sys.argv[1])
    except IndexError:
        pass
    WebSocketServer((config['listen'], config['port']),
            resources).serve_forever()
