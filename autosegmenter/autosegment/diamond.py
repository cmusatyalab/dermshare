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

import numpy as np

from opendiamond.filter import Filter
from PIL import Image
from skimage.transform import rescale

from .util import watchdog, make_boring
from .image import resize
from .hairremover import HairRemover
from .segmenter import Segmenter

TIMEOUT = 300
DEFAULT_IMAGE_SIZE = (640, 640)
HEATMAP_NAME = '_filter.%s.heatmap.png'

def processOneImage(image, debug=None):
    # set a 5 minute execution timeout
    with watchdog(TIMEOUT):
        make_boring()

        image, scale = resize(image, DEFAULT_IMAGE_SIZE)
        if debug:
            debug['scaled_original'] = image

        print "=================\nRemoving hair ..."
        hair_removed = HairRemover(image, debug)

        print "=================\nSegmenting ..."
        segmentation = Segmenter(hair_removed, debug)
        return rescale(segmentation, 1./scale, order=3)

    return None

class AutoSegmentationFilter(Filter):
    def __call__(self, obj):
        results = processOneImage(obj.image)

        segmentation = results['segmentation']
        segmentation = Image.fromarray(np.uint8(segmentation * 255.))
        obj.set_heatmap(HEATMAP_NAME % self.session.name, segmentation)
        return True

