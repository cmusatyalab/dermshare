#!/usr/bin/env python
#
# Create Diamond filter and predicate
#
# Copyright (c) 2012, 2015, 2016 Carnegie Mellon University
# All rights reserved.
#
# This software is distributed under the terms of the Eclipse Public
# License, Version 1.0 which can be found in the file named LICENSE.
# ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS SOFTWARE CONSTITUTES
# RECIPIENT'S ACCEPTANCE OF THIS AGREEMENT
#

import glob
import os
from zipfile import ZipFile, ZipInfo, ZIP_DEFLATED
from opendiamond.bundle import parse_manifest, bundle_generic

def zipwrite(archive, filename, arcname=None):
    with open(filename, 'rb') as f:
        zi = ZipInfo(arcname or filename)
        zi.compress_type = ZIP_DEFLATED
        archive.writestr(zi, f.read())

# create Gemini filter
with open('fil_autosegment', 'w+b') as f:
    f.write("#!/usr/bin/env python\n")

    diamond_filter = ZipFile(f, 'a', compression=ZIP_DEFLATED)
    zipwrite(diamond_filter, 'filter_loader.py', '__main__.py')
    zipwrite(diamond_filter, 'requirements.txt')
    for pyfile in sorted(glob.glob('autosegment/*.py')):
        zipwrite(diamond_filter, pyfile)
    diamond_filter.close()

    os.fchmod(f.fileno(), 0755)

# create Gemini predicate
manifest = parse_manifest(open('autosegment.xml').read())
bundle_generic('autosegment.pred', manifest, dict(fil_autosegment='fil_autosegment'))

