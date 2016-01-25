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

from skimage.color import rgb2lab, lab2rgb
from skimage.filter.rank import median
from skimage.morphology import closing, dilation, disk, square
from skimage.transform import rescale
from skimage.util import img_as_ubyte

#
# Aspect preserving resize that also returns the scale factor
#
def resize(image, max_size, order=3):
    scale_x = float(max_size[0]) / image.shape[1]
    scale_y = float(max_size[1]) / image.shape[0]
    scale = min(scale_x, scale_y)
    image = img_as_ubyte(rescale(image, scale, order=order))
    return image, scale

#
# Convert to/from CIElab color space (normalized to [0-1] values)
#
CIELAB_OFFSET = (0, 87, 108)
CIELAB_SCALE = (100, 186, 203)

def rgb2labnorm(rgb_image):
    lab_image = rgb2lab(rgb_image)
    return (lab_image + CIELAB_OFFSET) / CIELAB_SCALE

def labnorm2rgb(labnorm_image):
    lab_image = (labnorm_image[...,:3] * CIELAB_SCALE) - CIELAB_OFFSET
    return lab2rgb(lab_image)

# morphological closing operator
def morph_close(image, selem=None):
    if selem is None:
        # default mask is 11x11 square with rounded corners
        selem = disk(6)[1:-1,1:-1]
    return closing(image, selem)

def morph_dilate(image, selem=None):
    if selem is None:
        # default mask is 3x3 disk/cross
        selem = disk(1)
    return dilation(image, selem)

# median filter, we filter each RGB or LAB channel separately and merge back
def median_filter(image, selem=None):
    if selem is None:
        # default mask is 5x5 square
        selem = square(5)
    depth = image.shape[2]
    return np.dstack(median(channel[...,0], selem)
                     for channel in np.dsplit(image, depth)) / 255.

