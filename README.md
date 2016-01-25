# Installing

## Requirements

### At build time

- [OpenDiamond](http://diamond.cs.cmu.edu/)

### On the web application server

- OpenDiamond

### On the Diamond servers

- 64-bit environment
- OpenDiamond
- virtualenv
- OpenCV-Python
- BLAS library and headers
- LAPACK library and headers
- freetype library and headers
- libpng library and headers
- libjpeg library and headers

## DermShare

1. Use [make-filter.py](autosegmenter/make-filter.py) to build the Autosegment filter.
2. Use [make-filter.py](gemini/make-filter.py) to build the Gemini filter.
3. Create and populate virtualenv.  Use `--system-site-packages`, since
   OpenDiamond can't be installed with pip.
4. Configure and run.  `dermshare.py` is always needed, but `dermshare-ws.py`
   is only necessary if the mobile device link is enabled with
   `WEBSOCKET_BASEURL`.

## Diamond servers

To avoid conflicts with older versions of NumPy and Six packaged by your
Linux distribution, it is best to (re-)install NumPy v1.9.1 and Six v1.9.0
with pip after installing OpenCV-Python.

`fil_autosegment` and `fil_gemini` must install dependencies into a virtualenv
in /tmp the first time it runs.  To avoid long delays in the first search, the
virtualenv can be prepopulated by running `fil_autosegment` and `fil_gemini`
without arguments on the Diamond servers.  To avoid additional delays whenever
a Diamond server is rebooted or /tmp is cleared, you can globally install the
necessary Python libraries:

    pip install -r autosegment/requirements.txt
    pip install -r gemini/requirements.txt

## Scope server

An example scope server is provided in [simplescope](simplescope).  The
scope server shipped with OpenDiamond will *not* work because it doesn't
integrate with Diamond web apps.

## Dataretriever

An example dataretriever is provided in [dataretriever](dataretriever).  It
includes support for serving segmentation images and pathology reports
in addition to the Diamond objects and scope lists.  The `diamond_store`
dataretriever shipped with OpenDiamond will probably also work, but has not
been tested.

### Laying out the data

The DermShare dataretriever expects a directory tree with the following
layout:

```
DERMSHARE_SCOPE_PATH
    <scope1>
        <name1>-image.{jpg|png|...}
        <name1>-segm.png
        <name1>-attr.yaml
        <name1>-path.html
        <name2>-image.{jpg|png|...}
        ...
    <scope2>
        ...
```

The segmentation image must be a PNG the same size as the image, with
the pixels corresponding to the lesion marked in red and the remaining
pixels transparent.

The attributes file, if present, contains only one attribute, the
diagnosis code as listed in [search.js](app/static/search.js):

```
diagnosis: bcc
```

The path file, if present, contains the pathology report to be shown in
DermShare.
