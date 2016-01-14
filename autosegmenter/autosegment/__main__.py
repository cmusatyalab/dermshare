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

parser = argparse.ArgumentParser()
parser.add_argument('-d', '--debug', action="store_true")
parser.add_argument('image')


def interactive_eval(image_path):
    image = imread(image_path)
    debug = {}

    processOneImage(image, debug)

    # dump intermediate results for debug
    base_path = os.path.splitext(image_path)[0]
    for key, value in debug.items():
        if isinstance(value, np.ndarray):
            imsave("{}_{}.png".format(base_path, key), value)

if __name__ == '__main__':
    args = parser.parse_args()

    if not args.debug:
        warnings.simplefilter("ignore")

    if args.image == '--filter':
        AutoSegmentationFilter.run()
    else:
        interactive_eval(args.image)

