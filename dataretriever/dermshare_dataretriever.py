#!/usr/bin/env python
#
# DermShare dataretriever - An OpenDiamond dataretriever for DermShare datasets
#
# Copyright (c) 2012-2015 Carnegie Mellon University
# All rights reserved.
#
# This software is distributed under the terms of the Eclipse Public
# License, Version 1.0 which can be found in the file named LICENSE.
# ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS SOFTWARE CONSTITUTES
# RECIPIENT'S ACCEPTANCE OF THIS AGREEMENT
#

from flask import Flask, url_for, abort, Response, make_response, send_file, safe_join
from optparse import OptionParser
import os
import re
import yaml
import xml.etree.ElementTree as et

### Configurable settings
# directory that contains the scope directories
DERMSHARE_SCOPE_PATH = '.'

app = Flask(__name__)
app.config.from_object(__name__)
app.config.from_envvar('DATARETRIEVER_SETTINGS', True)


image_tmpl = "{object_id}-image{ext}"
segm_tmpl = "{object_id}-segm.png"
attrs_tmpl = "{object_id}-attr.yaml"
path_tmpl = "{object_id}-path.html"


# helper to safely prefix path with the currently specified scope
def scopeme(scope, path=None):
    scopedir = safe_join(app.config['DERMSHARE_SCOPE_PATH'], scope)
    if not path: return scopedir
    return safe_join(scopedir, path)


@app.route('/<anything>')
def nullroute(*args, **kwargs):
    abort(404)

@app.route('/<scope>/', defaults={'N': 0, 'M': 1})
@app.route('/<scope>/<int:M>/<int:N>/')
def scopelist(scope, N, M):
    # do some input validation
    if M < 1 or N < 0 or N >= M:
        abort(400)

    try:
        objects = []
        for name in os.listdir(scopeme(scope)):
            m = re.match('(.*)-image\.([a-zA-Z]{3,4})$', name)
            if not m: continue
            object_id, ext = m.groups()
            objects.append((object_id, ext))
    except IOError:
        abort(404)

    # make sure we have a somewhat predictable ordering between runs
    objects.sort()

    # slice and dice the scopelist
    if M != 1: objects = objects[N::M]

    # convert to XML etree
    scopelist = et.Element('objectlist', count=str(len(objects)))
    for object_id, ext in objects:
        object_url = url_for('.object', scope=scope, object_id=object_id, ext=ext)
        scopelist.append(et.Element('object', src=object_url))

    return Response(et.tostring(scopelist), mimetype='text/xml')

@app.route('/<scope>/obj/<object_id>.<ext>')
def object(scope, object_id, ext):
    image_file = image_tmpl.format(object_id=object_id, ext='.'+ext)
    image_path = scopeme(scope, image_file)
    if not os.path.exists(image_path):
        abort(404)

    attrs_file = attrs_tmpl.format(object_id=object_id)
    attrs_path = scopeme(scope, attrs_file)
    try:
        with open(attrs_path) as f:
            attrs = yaml.safe_load(f)
    except IOError:
        attrs = {}

    resp = make_response(send_file(image_path))
    resp.headers.extend( ('x-attr-dermshare.{0}'.format(key.lower()), value)
                         for key, value in attrs.items() )

    segm_file = segm_tmpl.format(object_id=object_id)
    segm_path = scopeme(scope, segm_file)
    if os.path.exists(segm_path):
        segmentation_url = url_for('.segmentation', scope=scope, object_id=object_id,
                                   _external=True)
        resp.headers['x-attr-dermshare.segmentation-url'] = segmentation_url

    path_file = path_tmpl.format(object_id=object_id)
    path_path = scopeme(scope, path_file)
    if os.path.exists(path_path):
        pathreport_url = url_for('.pathreport', scope=scope, object_id=object_id,
                                 _external=True)
        resp.headers['x-attr-dermshare.pathology-report-url'] = pathreport_url

    return resp

@app.route('/<scope>/obj/<object_id>/segm.png')
def segmentation(scope, object_id):
    segm_file = segm_tmpl.format(object_id=object_id)
    segm_path = scopeme(scope, segm_file)
    return send_file(segm_path)

@app.route('/<scope>/obj/<object_id>/path.html')
def pathreport(scope, object_id):
    path_file = path_tmpl.format(object_id=object_id)
    path_path = scopeme(scope, path_file)

    resp = make_response(send_file(path_path))
    resp.headers['Access-Control-Allow-Origin'] = '*'
    return resp

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
                type='int', default=5002,
                help='port to listen on [5002]')

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
