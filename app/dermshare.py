#!/usr/bin/env python
#
# DermShare -- application server
#
# Copyright (c) 2012-2015 Carnegie Mellon University
# All rights reserved.
#
# This software is distributed under the terms of the Eclipse Public
# License, Version 1.0 which can be found in the file named LICENSE.
# ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS SOFTWARE CONSTITUTES
# RECIPIENT'S ACCEPTANCE OF THIS AGREEMENT
#

DERMSHARE_VERSION = '1.0'
DERMSHARE_DATE = '2015-06-01'

import base64
from contextlib import contextmanager
from flask import Flask, render_template, request, Response, url_for
from flask.ext.assets import Environment, Bundle
from functools import wraps
from hashlib import sha256
import json
from opendiamond.scope import ScopeCookie, get_blaster_map
from optparse import OptionParser
import os
import requests
import subprocess
import tempfile
import threading
import time
import urllib
from urlparse import urljoin, urlsplit
import uuid
from werkzeug.exceptions import BadRequest as _BadRequest

WEBSOCKET_BASEURL = None
SCOPESERVER = 'https://scopeserver.diamond.example.org/'
AUTOSEGMENT_COOKIE = '/etc/dermshare/autosegment-cookie'
DUMP_SEARCH_DATA = True

app = Flask(__name__)
app.config.from_object(__name__)
app.config.from_envvar('DERMSHARE_SETTINGS', True)

assets = Environment(app)

app_css = Bundle(
    '3rdparty/jqplot/jquery.jqplot.css',
    '3rdparty/bootstrap/css/bootstrap.css',
    '3rdparty/fontawesome/css/font-awesome.css',
    filters='cssrewrite'
)
assets.register('app_css', Bundle(
    app_css,
    filters='cssmin', output='gen/app.css'
))
remote_css = Bundle(
    '3rdparty/bootstrap/css/bootstrap.css',
    '3rdparty/fontawesome/css/font-awesome.css',
    filters='cssrewrite'
)
assets.register('remote_css', Bundle(
    remote_css,
    filters='cssmin', output='gen/remote.css'
))

app_js_part1 = Bundle(
    # Check browser support first, in case the browser is too old to be
    # supported by jQuery 2
    'browser.js',
    '3rdparty/jquery.js',
    '3rdparty/jqplot/jquery.jqplot.js',
    '3rdparty/jqplot/jqplot.barRenderer.js',
    '3rdparty/jqplot/jqplot.categoryAxisRenderer.js',
    '3rdparty/jqplot/jqplot.categoryAxisRenderer.js',
    '3rdparty/jqplot/jqplot.highlighter.js',
    '3rdparty/bootstrap/js/bootstrap.js',
    '3rdparty/kinetic.js',
    '3rdparty/sockjs.js',
    '3rdparty/json-blaster.js',
    filters='rjsmin'
)
app_js_part2 = Bundle(
    'util.js',
    'search.js',
    'result.js',
    'segment.js',
    'import.js',
    'plot.js',
    'websocket.js',
    'app.js',
    filters='rjsmin',
)
assets.register('app_js', Bundle(
    app_js_part1,
    '3rdparty/knockout.min.js',
    app_js_part2,
    output='gen/app.js'
))
remote_js_part1 = Bundle(
    'browser.js',
    '3rdparty/jquery.js',
    '3rdparty/bootstrap/js/bootstrap.js',
    filters='rjsmin'
)
remote_js_part2 = Bundle(
    'import.js',
    'websocket.js',
    'remote.js',
    filters='rjsmin',
)
assets.register('remote_js', Bundle(
    remote_js_part1,
    '3rdparty/knockout.min.js',
    remote_js_part2,
    output='gen/remote.js'
))


http = requests.Session()

class HashCache(object):
    BUFSIZE = 10 << 20

    def __init__(self):
        self._lock = threading.Lock()
        self._cache = {}  # path -> (stat result, sha256)

    def __getitem__(self, path):
        stat = os.stat(path)
        with self._lock:
            if path in self._cache:
                cached_stat, cached_hash = self._cache[path]
                if ((stat.st_dev, stat.st_ino, stat.st_size,
                        stat.st_mtime) == (cached_stat.st_dev,
                        cached_stat.st_ino, cached_stat.st_size,
                        cached_stat.st_mtime)):
                    return cached_hash
        digest = sha256()
        with open(path, 'rb') as fh:
            while True:
                buf = fh.read(self.BUFSIZE)
                if not buf:
                    break
                digest.update(buf)
        hash = digest.hexdigest()
        with self._lock:
            self._cache[path] = (stat, hash)
        return hash


hash_cache = HashCache()


class BadRequest(_BadRequest):
    '''Flask BadRequest subclass that produces error output as plain text.'''

    def get_body(self, environ):
        return self.description

    def get_headers(self, environ):
        return [('Content-Type', 'text/plain')]


@contextmanager
def http_activity(activity):
    '''Format Requests exceptions and convert them to BadRequest.'''
    try:
        yield
    except requests.exceptions.HTTPError, e:
        raise BadRequest('Error %s: %s' % (activity, e.response.text))
    except requests.exceptions.RequestException, e:
        raise BadRequest('Error %s: %s' % (activity, e))


@app.before_first_request
def _setup():
    # Get version and date
    gitdir = os.path.dirname(os.path.dirname(os.path.realpath(__file__)))
    cmds = (
        ('version', ['git', 'describe', '--dirty', '--always'],
                'v' + DERMSHARE_VERSION),
        ('date', ['git', 'log', '-1', '--format=format:%cd', '--date=short'],
                DERMSHARE_DATE),
    )
    for attr, cmd, default in cmds:
        if os.path.isdir(os.path.join(gitdir, '.git')):
            proc = subprocess.Popen(cmd, cwd=gitdir, close_fds=True,
                    stdout=subprocess.PIPE)
            out, _ = proc.communicate()
            if proc.returncode:
                setattr(app, attr, 'UNKNOWN')
            else:
                setattr(app, attr, out.strip())
        else:
            setattr(app, attr, default)

    # Set User-Agent for outgoing requests
    http.headers['User-Agent'] = 'DermShare/%s %s' % (
        app.version.split('-')[0].lstrip('v'),
        http.headers['User-Agent'],
    )


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


def _get_ws_url(endpoint):
    url = app.config['WEBSOCKET_BASEURL']
    if not url:
        return None
    # urljoin doesn't work for ws/wss URLs
    return url + ('' if url.endswith('/') else '/') + endpoint


@app.route('/', methods=('GET', 'POST'))
def index():
    # No CSRF protection on POST, since we expect cross-site cookie
    # submissions
    return render_template('index.html',
        debug=app.config['DEBUG'],
        app_version=app.version,
        app_date=app.date,
        csrf_token=request.csrf_token,
        scopeserver=app.config['SCOPESERVER'],
        ws_url=_get_ws_url('client') or '',
        cookie=request.form.get('cookie'),
        description=request.form.get('description'),
    )


@app.route('/unsupported')
def unsupported():
    return render_template('unsupported.html')


@app.route('/remote/<token>')
def remote(token):
    return render_template('remote.html',
        ws_url=_get_ws_url('mobile') or '',
        token=token,
    )


def _blaster_url(cookie):
    '''Cookie is a string, possibly representing a megacookie.  For now,
    raise an exception if there are multiple cookies pointing to different
    blasters.'''
    map = get_blaster_map([ScopeCookie.parse(c)
            for c in ScopeCookie.split(cookie)])
    if not map:
        raise BadRequest('No JSON blaster specified in scope cookies')
    if len(map) > 1:
        raise BadRequest('Multiple JSON blasters not supported')
    return map.keys()[0]


def _search_blob_spec(name):
    hash = hash_cache[os.path.join(app.static_folder, name)]
    return {
        # Defeat caching when the filter changes
        'uri': '%s?h=%s' % (
            url_for('static', filename=name, _external=True),
            hash[:8],
        ),
        'sha256': hash,
    }


def _get_segm_filters():
    return [
        {
            'name': 'RGB',
            'code': _search_blob_spec('fil_rgb'),
        },
        {
            'name': 'autosegmenter',
            'code': _search_blob_spec('autosegmenter'),
            'dependencies': ['RGB'],
            'min_score': 1,
        },
    ]


def _get_gemini_example_filters(segm_uri):
    return [
        {
            'name': 'RGB',
            'code': _search_blob_spec('fil_rgb'),
        },
        {
            'name': 'segmentation',
            'code': _search_blob_spec('fil_gemini'),
            'blob': {
                'uri': segm_uri,
            },
            'arguments': [
                'FixedSegmentationFilter',
                'true',
                'false',
                'false',
                'false',
            ],
            'min_score': 1,
        },
        {
            'name': 'gemini',
            'code': _search_blob_spec('fil_gemini'),
            'arguments': [
                'GeminiFilter',
                'segmentation',
            ],
            'dependencies': ['RGB', 'segmentation'],
        },
    ]


def _get_gemini_filters(example_result):
    feature = lambda name: example_result['gemini.gemini.' + name]['data']
    return [
        {
            'name': 'RGB',
            'code': _search_blob_spec('fil_rgb'),
        },
        {
            'name': 'thumbnail',
            'code': _search_blob_spec('fil_fixedthumb'),
            'arguments': [
                'thumbnail.jpeg',
                '150',
            ],
            'dependencies': ['RGB'],
        },
        {
            'name': 'large-thumbnail',
            'code': _search_blob_spec('fil_fixedthumb'),
            'arguments': [
                'large-thumbnail.jpeg',
                '400',
            ],
            'dependencies': ['RGB'],
        },
        {
            'name': 'segmentation',
            'code': _search_blob_spec('fil_gemini'),
            'arguments': [
                'SegmentationFilter',
                'true',
                'false',
                'false',
                'false',
            ],
            'min_score': 1,
        },
        {
            'name': 'gemini',
            'code': _search_blob_spec('fil_gemini'),
            'arguments': [
                'GeminiFilter',
                'segmentation',
            ],
            'dependencies': ['RGB', 'segmentation'],
        },
        {
            'name': 'distance',
            'code': _search_blob_spec('fil_gemini'),
            'arguments': [
                'DistanceFilter',
                'gemini',
                feature('border_scores_average'),
                feature('border_scores_stddev'),
                feature('color_asymmetry_major_emd'),
                feature('color_asymmetry_minor_emd'),
                feature('color_histogram_black'),
                feature('color_histogram_blue_gray'),
                feature('color_histogram_dark_brown'),
                feature('color_histogram_light_brown_tan'),
                feature('color_histogram_red'),
                feature('color_histogram_white_pink'),
                feature('color_shape_asymmetry_mean'),
                feature('color_shape_asymmetry_stddev'),
                feature('colors_histogram_average'),
                feature('colors_histogram_nonzero'),
                feature('colors_histogram_variance'),
                feature('colors_pvsc_density_baysian_l'),
                feature('colors_pvsc_density_baysian_a'),
                feature('colors_pvsc_density_baysian_b'),
                feature('colors_pvsc_mean_difference_l'),
                feature('colors_pvsc_mean_difference_a'),
                feature('colors_pvsc_mean_difference_b'),
                feature('geometric_p25'),
                feature('geometric_p50'),
                feature('geometric_p75'),
                feature('lesion_area'),
                feature('lesion_area_filled'),
                feature('lesion_eccentricity'),
                feature('lesion_formfactor'),
                feature('lesion_intensity_max'),
                feature('lesion_intensity_mean'),
                feature('lesion_intensity_min'),
                feature('lesion_major_axis_length'),
                feature('lesion_minor_axis_length'),
                feature('lesion_perimeter'),
                feature('lesion_solidity'),
                feature('local_binary_pattern_0'),
                feature('local_binary_pattern_1'),
                feature('local_binary_pattern_2'),
                feature('local_binary_pattern_3'),
                feature('local_binary_pattern_4'),
                feature('local_binary_pattern_5'),
                feature('local_binary_pattern_6'),
                feature('local_binary_pattern_7'),
                feature('local_binary_pattern_8'),
                feature('scharr_0'),
                feature('scharr_1'),
                feature('scharr_2'),
                feature('scharr_3'),
                feature('scharr_4'),
                feature('scharr_5'),
                feature('scharr_6'),
                feature('scharr_7'),
                feature('shape_asymmetry_major_ratio'),
                feature('shape_asymmetry_minor_ratio'),
                feature('delphi_score'),
            ],
            'dependencies': ['gemini'],
        },
    ]


def _make_search(cookie, filters):
    config = {
        'cookies': [cookie],
        'filters': filters,
    }
    with http_activity('starting search'):
        response = http.post(_blaster_url(cookie), data=json.dumps(config),
                headers={'Content-Type': 'application/json'})
        response.raise_for_status()
    try:
        return response.json()
    except ValueError:
        raise BadRequest('Error starting search: invalid JSON')


def _send_blob(cookie, desc, data):
    with http_activity('sending %s' % desc):
        response = http.post(urljoin(_blaster_url(cookie), 'blob'),
                data=data,
                headers={'Content-Type': 'application/octet-stream'})
        response.raise_for_status()
    return response.headers['Location']


def _evaluate_object(search_info, uri):
    evaluate = {
        'object': {
            'uri': uri,
        },
    }
    with http_activity('evaluating object'):
        response = http.post(search_info['evaluate_url'],
                data=json.dumps(evaluate),
                headers={'Content-Type': 'application/json'})
        response.raise_for_status()
    try:
        result = response.json()
        if '' not in result:
            # Object dropped by filters
            raise ValueError
    except ValueError:
        # Object dropped or invalid JSON
        raise BadRequest('Image could not be processed.')
    return result


def _extract_file(request, form_field):
    f = request.files[form_field]
    if hasattr(f, 'getvalue') and not hasattr(f, 'len'):
        # The file is small (< 500 KB) so Werkzeug stored its data in
        # memory.  Requests will not be able to detect its size.  (Werkzeug
        # stores larger files in a TemporaryFile, so Requests can use
        # fileno() and os.fstat().)  We need to help Requests figure out the
        # file size so it doesn't upload using chunked Transfer-Encoding,
        # which would cause the JSON Blaster to choke.  (Tornado and older
        # versions of nginx can't dechunk.)
        f.len = len(f.getvalue())
    return f


def _decode_data_uri(uri):
    parts = urlsplit(uri)
    if parts.scheme != 'data':
        raise BadRequest('Unsupported scheme for data URI')
    metadata, data = parts.path.split(',', 1)
    metadata_items = metadata.split(';')
    if 'base64' in metadata_items:
        return base64.b64decode(data)
    else:
        return urllib.unquote(data)


@app.route('/segment', methods=('POST',))
@_csrf_protect
def segment():
    # Parse request
    # We must read the request body before returning an error, otherwise
    # the client JS won't receive the error message.
    try:
        example = _extract_file(request, 'example')
    except KeyError:
        raise BadRequest('No example image provided')

    # Load scope cookie for autosegmentation
    with open(app.config['AUTOSEGMENT_COOKIE']) as fh:
        cookie = fh.read()

    # Send blob to blaster
    example_uri = _send_blob(cookie, 'example', example)

    # Create search
    search_info = _make_search(cookie, _get_segm_filters())

    # Send evaluate request
    result = _evaluate_object(search_info, example_uri)

    # Return response
    ret = {
        'url': result['_filter.autosegmenter.heatmap.png']['image_url'] +
                '?tint=ff0000',
    }
    return Response(json.dumps(ret), mimetype='application/json')


def _dump_search_data(example, segmentation, example_result):
    dumpdir = tempfile.mkdtemp(prefix='dermshare-search-%d-' % time.time())
    # Example image
    example.stream.seek(0)
    example.save(os.path.join(dumpdir, 'example'))
    # Client-provided segmentation
    with open(os.path.join(dumpdir, 'segmentation'), 'w') as fh:
        fh.write(segmentation)
    # Post-processed segmentation
    mask_url = example_result['_filter.segmentation.heatmap.png']['raw_url']
    r = requests.get(mask_url, stream=True)
    if r.status_code == 200:
        with open(os.path.join(dumpdir, 'mask'), 'w') as fh:
            for buf in r.iter_content():
                fh.write(buf)
    # Evaluation result
    with open(os.path.join(dumpdir, 'result'), 'w') as fh:
        json.dump(example_result, fh, indent=2, sort_keys=True)
    # Features
    features = {}
    for k, v in example_result.items():
        if k.startswith('gemini.gemini.'):
            features[k.replace('gemini.gemini.', '')] = float(v['data'])
    with open(os.path.join(dumpdir, 'features'), 'w') as fh:
        json.dump(features, fh, indent=2, sort_keys=True)


@app.route('/search', methods=('POST',))
@_csrf_protect
def search():
    # Parse request
    params = request.form
    try:
        # Fix mangled newlines in scope cookie
        cookie = params['scope'].replace('\r\n', '\n')
        example = _extract_file(request, 'example')
        segmentation = _decode_data_uri(params['segmentation'])
    except KeyError:
        raise BadRequest('Bad request parameters')

    # Evaluate Gemini on example
    segmentation_uri = _send_blob(cookie, 'segmentation', segmentation)
    filters = _get_gemini_example_filters(segmentation_uri)
    search_info = _make_search(cookie, filters)
    example_uri = _send_blob(cookie, 'example', example)
    example_result = _evaluate_object(search_info, example_uri)

    # Dump search parameters for debugging
    if app.config['DEBUG'] and app.config['DUMP_SEARCH_DATA']:
        _dump_search_data(example, segmentation, example_result)

    # Create search
    try:
        filters = _get_gemini_filters(example_result)
    except KeyError:
        raise BadRequest('Bad request parameters')
    search_info = _make_search(cookie, filters)

    # Respond
    response = {
        'socket_url': search_info['socket_url'],
        'search_key': search_info['search_key'],
        'example_attrs': example_result,
    }
    return Response(json.dumps(response), mimetype='application/json')


if __name__ == '__main__':
    parser = OptionParser(usage='Usage: %prog [options]')
    parser.add_option('-c', '--config', metavar='FILE', dest='config',
                help='config file')
    parser.add_option('-d', '--debug', dest='DEBUG', action='store_true',
                help='run in debugging mode (insecure)')
    parser.add_option('-i', '--cookie', metavar='PATH', dest='AUTOSEGMENT_COOKIE',
                help='scope cookie for performing segmentation')
    parser.add_option('-l', '--listen', metavar='ADDRESS', dest='host',
                default='127.0.0.1',
                help='address to listen on [127.0.0.1]')
    parser.add_option('-p', '--port', metavar='PORT', dest='port',
                type='int', default=5000,
                help='port to listen on [5000]')
    parser.add_option('-s', '--scopeserver', metavar='URL', dest='SCOPESERVER',
                help='embedding URL for scope server')
    parser.add_option('-w', '--websocket', metavar='BASE-URL',
                dest='WEBSOCKET_BASEURL',
                help='base URL for websocket server')

    parser.add_option('--debug-assets', dest='_debug_assets', action='store_true',
                help='debug asset bundling')
    parser.add_option('--build-assets', dest='_build_assets', action="store_true",
                help='bundle assets')

    (opts, args) = parser.parse_args()
    # Load config file if specified
    if opts.config is not None:
        app.config.from_pyfile(opts.config)
    # Overwrite only those settings specified on the command line
    for k in dir(opts):
        if not k.startswith('_') and getattr(opts, k) is None:
            delattr(opts, k)
    app.config.from_object(opts)

    if opts._debug_assets:
        app.config['DEBUG'] = True
        app.config['ASSETS_DEBUG'] = False
    else:
        app.config['ASSETS_DEBUG'] = app.config['DEBUG']

    if opts._build_assets:
        from webassets import script
        script.main(['build', '--production'], env=assets)
    else:
        app.run(host=opts.host, port=opts.port, threaded=True)
