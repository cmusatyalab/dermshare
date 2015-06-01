#
# Gemini -- texture based feature extraction
#
# Copyright (c) 2014-2015 Carnegie Mellon University
# All rights reserved.
#
# This software is distributed under the terms of the Eclipse Public
# License, Version 1.0 which can be found in the file named LICENSE.
# ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS SOFTWARE CONSTITUTES
# RECIPIENT'S ACCEPTANCE OF THIS AGREEMENT
#

from skimage.color import rgb2gray
from skimage.feature import hog, local_binary_pattern
from skimage.filter import sobel, scharr
import numpy as np

def delphi_hog(image_roi, attrs={}, debug=False):
    greyscale = rgb2gray(image_roi)

    HOG_labels = hog(greyscale)

    if debug:
        print "HOG min/max", HOG_labels.min(), HOG_labels.max()
    H = np.bincount(HOG_labels)

    attrs.update(('hog%d' % i, v) for i, v in enumerate(H))
    return attrs


def delphi_lbp(image_roi, attrs={}, debug=False):
    greyscale = rgb2gray(image_roi)

    # Local Binary Pattern settings from the Zortea paper
    METHOD = 'uniform'  # rotation invariance with uniform patterns
    P = 8               # sampling points
    R = 2.0             # radius

    LBP_labels = local_binary_pattern(greyscale, P, R, METHOD)

    #patches = image.extract_patches_2d(LBP_labels, (41,41), max_patches=0.0025)
    #for patch in patches:
    #    H = np.bincount(patch)

    H = np.histogram(LBP_labels, bins=P+1, range=(0,P+1), density=True)[0]

    if debug:
        print "=== Local Binary Patterns ==="
        print H
        print

    attrs.update(('Local Binary Pattern %d' % i, v) for i, v in enumerate(H))
    return attrs


def delphi_sobel(image_roi, attrs={}, debug=False):
    greyscale = rgb2gray(image_roi)

    edge_magnitude = sobel(greyscale)

    H = np.histogram(edge_magnitude, bins=8, range=(0,1), density=True)[0]

    if debug:
        print "=== Sobel Texture Histogram ==="
        print H
        print

    attrs.update(('sobel%d' % i, v) for i, v in enumerate(H))
    return attrs

def delphi_scharr(image_roi, attrs={}, debug=False):
    greyscale = rgb2gray(image_roi)

    edge_magnitude = scharr(greyscale)

    H = np.histogram(edge_magnitude, bins=8, range=(0,1), density=True)[0]

    if debug:
        print "=== Scharr Texture Histogram ==="
        print H
        print

    attrs.update(('Scharr %d' % i, v) for i, v in enumerate(H))
    return attrs

