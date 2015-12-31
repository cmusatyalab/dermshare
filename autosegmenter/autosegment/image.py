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

import numpy as np

from skimage.color import rgb2lab, lab2rgb
from skimage.filters import median
from skimage.morphology import closing, dilation, disk
from skimage.transform import rescale
from skimage.util import img_as_ubyte, pad

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

#
# Helpers to apply morphological operations on an image with a padded border
#
def _MorphWithBorder(op, img, mask):
    N = mask.shape[0]/2
    padded = pad(img, N, 'edge')
    result = op(padded, mask)
    return result[N:-N,N:-N]

def morph_close(image, mask=disk(6)[1:-1,1:-1]):
    # default mask is 11x11 square with rounded corners
    return _MorphWithBorder(closing, image, mask)

def morph_dilate(image, mask=disk(1)):
    return _MorphWithBorder(dilation, image, mask)

def median_filter(image, selem):
    depth = image.shape[2]
    return np.dstack(_MorphWithBorder(median, channel[...,0], selem)
                     for channel in np.dsplit(image, depth)) / 255.

