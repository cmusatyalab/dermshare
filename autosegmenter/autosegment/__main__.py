#!/usr/bin/env python
#
# DermShare Autosegmenter
#
# Copyright (c) 2015,2016 Carnegie Mellon University
# All rights reserved.
#
# This software is distributed under the terms of the Eclipse Public
# License, Version 1.0 which can be found in the file named LICENSE.
# ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS SOFTWARE CONSTITUTES
# RECIPIENT'S ACCEPTANCE OF THIS AGREEMENT

import argparse
import os
import warnings
import numpy as np
from skimage.io import imread, imsave

from .diamond import AutoSegmentationFilter, processOneImage


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('-d', '--debug', action="store_true")
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument('--filter', action="store_true",
                       help="run as Diamond filter")
    group.add_argument('image', nargs='?',
                       help="image to segment")
    args = parser.parse_args()

    if not args.debug:
        warnings.simplefilter("ignore")

    if args.filter:
        AutoSegmentationFilter.run()

    # Not running as a diamond filter
    # process image and write out intermediate results as png
    image = imread(args.image)
    debug = {}

    processOneImage(image, debug)

    base_path = os.path.splitext(args.image)[0]
    for key, value in debug.items():
        if isinstance(value, np.ndarray):
            imsave("%s_%s.png" % (base_path, key), value)


if __name__ == '__main__':
    main()
