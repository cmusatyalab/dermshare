#!/usr/bin/env python
#
# SimpleScope -- simple Diamond scope server
#
# Copyright (c) 2012-2015 Carnegie Mellon University
# All rights reserved.
#
# This software is distributed under the terms of the Eclipse Public
# License, Version 1.0 which can be found in the file named LICENSE.
# ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS SOFTWARE CONSTITUTES
# RECIPIENT'S ACCEPTANCE OF THIS AGREEMENT
#

from datetime import timedelta
from flask import Flask, render_template, request
from functools import wraps
import json
from opendiamond.scope import generate_cookie
from optparse import OptionParser
from urlparse import urljoin
import uuid
from werkzeug.exceptions import BadRequest

SCOPE_COOKIE_LIFETIME = 86400  # seconds
PRIVATE_KEY = None  # defaults to ~/.diamond/key.pem
BLASTER_URL = 'https://blaster.diamond.example.org/'
DIAMOND_SERVERS = [
    'a.diamond.example.org',
    'b.diamond.example.org',
    'c.diamond.example.org',
]
# only used in scopeserver-first flow
APPLICATION = 'https://dermshare.example.org/'

SCOPES = (
    ('Small Library', 'http://data1.diamond.example.org/small/'),
    ('Extended Set', 'http://data1.diamond.example.org/extended/'),
    ('Unbiopsied Benign', 'http://data2.diamond.example.org/benign/'),
)

app = Flask(__name__)
app.config.from_object(__name__)
app.config.from_envvar('SIMPLESCOPE_SETTINGS', True)


@app.before_request
def _csrf_get():
    request.csrf_token = request.cookies.get('csrf_token', str(uuid.uuid4()))


@app.after_request
def _csrf_set(response):
    response.set_cookie('csrf_token', request.csrf_token, max_age=3600*24*30)
    return response


def _csrf_protect(f):
    @wraps(f)
    def wrapper(*args, **kwargs):
        if request.method == 'POST':
            if request.form.get('csrf_token', None) != request.csrf_token:
                # Read and discard the request data to ensure the client
                # receives the error string
                _ = request.data
                raise BadRequest('Bad or missing CSRF token')
        return f(*args, **kwargs)
    return wrapper


@app.route('/', methods=('GET', 'POST'))
@_csrf_protect
def index():
    error = None
    if request.method == 'POST':
        scopeurls = []
        descriptions = []
        for k, v in sorted(request.form.items()):
            if k.startswith('scope_'):
                i = int(k.replace('scope_', ''))
                desc, url = app.config['SCOPES'][i]
                scopeurls.append(url)
                descriptions.append(desc)
        if scopeurls:
            servers = app.config['DIAMOND_SERVERS']
            cookies = []
            for i, server in enumerate(servers):
                server_scopeurls = [
                    urljoin(
                        u + ('' if u.endswith('/') else '/'),
                        '{0}/{1}/'.format(len(servers), i)
                    )
                    for u in scopeurls
                ]
                cookies.append(generate_cookie(
                    scopeurls=server_scopeurls,
                    servers=[server],
                    blaster=app.config['BLASTER_URL'],
                    keyfile=app.config['PRIVATE_KEY'],
                    expires=timedelta(
                        seconds=app.config['SCOPE_COOKIE_LIFETIME'],
                    ),
                ))
            return render_template('scope.html',
                application=app.config['APPLICATION'],
                scope_info=json.dumps({
                    'cookie': ''.join(cookies),
                    'description': ', '.join(descriptions)
                }),
            )
        else:
            error = 'Please select a scope.'

    return render_template('index.html',
        csrf_token=request.csrf_token,
        scopes=[a[0] for a in app.config['SCOPES']],
        error=error,
    )


if __name__ == '__main__':
    parser = OptionParser(usage='Usage: %prog [options]')
    parser.add_option('-c', '--config', metavar='FILE', dest='config',
                help='config file')
    parser.add_option('-d', '--debug', dest='DEBUG', action='store_true',
                help='run in debugging mode (insecure)')
    parser.add_option('-l', '--listen', metavar='ADDRESS', dest='host',
                default='127.0.0.1',
                help='address to listen on [127.0.0.1]')
    parser.add_option('-p', '--port', metavar='PORT', dest='port',
                type='int', default=5001,
                help='port to listen on [5001]')

    (opts, args) = parser.parse_args()
    # Load config file if specified
    if opts.config is not None:
        app.config.from_pyfile(opts.config)
    # Overwrite only those settings specified on the command line
    for k in dir(opts):
        if not k.startswith('_') and getattr(opts, k) is None:
            delattr(opts, k)
    app.config.from_object(opts)

    app.run(host=opts.host, port=opts.port, threaded=True)
