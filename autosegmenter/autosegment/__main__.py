#!/usr/bin/env python
#
# DermShare Autosegmenter
#
# Copyright (c) 2015 Carnegie Mellon University
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

from opendiamond.filter import Filter
from skimage.io import imread, imsave
from skimage.transform import rescale

from .util import watchdog, make_boring
from .image import resize
from .hairremover import HairRemover
from .segmenter import Segmenter

TIMEOUT = 300
DEFAULT_IMAGE_SIZE = (640, 640)
HEATMAP_NAME = '_filter.%s.heatmap.png'

parser = argparse.ArgumentParser()
parser.add_argument('-d', '--debug', action="store_true")
parser.add_argument('image')
args = parser.parse_args()


def processOneImage(image):
    results = {}

    # set a 5 minute execution timeout
    with watchdog(TIMEOUT):
        make_boring()

        image, scale = resize(image, DEFAULT_IMAGE_SIZE)
        results['scaled_original'] = image
        results['scale_factor'] = scale

        # removeHair
        print "=================\nRemoving hair ..."
        hair_removed = HairRemover(image, results)
        
        # segment
        print "=================\nSegmenting ..."
        results['segmented'] = Segmenter(hair_removed, results)

    return results

class AutoSegmentationFilter(Filter):
    def __call__(self, obj):
        results = processOneImage(obj.image)
        #diamond_context.save(ohandle)

        segmentation = results['segmented']
        scale = results['scale_factor']
        segmentation = rescale(segmentation, 1./scale, order=3)

        #segmentation = Image.fromarray(np.uint8(mask * 255.))
        obj.set_heatmap(HEATMAP_NAME % self.session.name, segmentation)
        return True

def interactive_eval(image_path):
    image = imread(image_path)
    results = processOneImage(image)

    #diamond_context.save("{}.text_attr".format(image_path))

    # dump intermediate results for debug
    base_path = os.path.splitext(image_path)[0]

    for key, value in results.items():
        if isinstance(value, np.ndarray):
            imsave("{}_{}.png".format(base_path, key), value)

def run_filter():
    Filter.run([AutoSegmentationFilter])

if __name__ == '__main__':
    if not args.debug:
        warnings.simplefilter("ignore")

    if args.image == '--filter':
        run_filter()
    else:
        interactive_eval(args.image)

