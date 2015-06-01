#
# Gemini -- Earth Mover's Distance
#
# Copyright (c) 2014-2015 Carnegie Mellon University
# All rights reserved.
#
# This software is distributed under the terms of the Eclipse Public
# License, Version 1.0 which can be found in the file named LICENSE.
# ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS SOFTWARE CONSTITUTES
# RECIPIENT'S ACCEPTANCE OF THIS AGREEMENT
#
# Wraps the OpenCV implementation
#
# @reference
#  Y. Rubner, C. Tomasi, and L. J. Guibas. A Metric for Distributions with
#  Applications to Image Databases. Proceedings of the 1998 IEEE International
#  Conference on Computer Vision, Bombay, India, January 1998, pp. 59-66.
#

import numpy as np
try:
    from cv2 import cv
    fromarray = cv.fromarray
except ImportError:
    # opencv-2.0.0 compatibility
    import cv
    def fromarray(array):
        mat = cv.CreateMat(array.shape[0], array.shape[1], cv.CV_32FC1)
        cv.SetData(mat, array, cv.CV_AUTOSTEP)
        return mat

MANHATTAN_DISTANCE = cv.CV_DIST_L1
EUCLIDEAN_DISTANCE = cv.CV_DIST_L2

def _make_signature(histogram):
    weight = 1.0 / histogram.shape[0]
    floats = histogram.astype('float32')
    signature = np.insert(floats, 0, weight, axis=1)
    return fromarray(signature)

def EarthMoversDistance(hist1, hist2):
    signature1 = _make_signature(hist1)
    signature2 = _make_signature(hist2)
    return cv.CalcEMD2(signature1, signature2, EUCLIDEAN_DISTANCE)

