#
# DermShare Autosegmenter
#
# Copyright (c) 2014-2015 Carnegie Mellon University
# All rights reserved.
#
# This software is distributed under the terms of the Eclipse Public
# License, Version 1.0 which can be found in the file named LICENSE.
# ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS SOFTWARE CONSTITUTES
# RECIPIENT'S ACCEPTANCE OF THIS AGREEMENT

#
# Wrap various OpenCV functions to make it easier to call from a codebase that
# primarily uses numpy/scikit-image based data.
#

import numpy as np
from skimage.util import img_as_ubyte
try:
    from cv2 import cv, inpaint, INPAINT_NS, INPAINT_TELEA
    fromarray = cv.fromarray

except ImportError:
    # opencv-2.0.0 compatibility
    import cv
    def fromarray(array):
        mat = cv.CreateMat(array.shape[0], array.shape[1], cv.CV_32FC1)
        cv.SetData(mat, array, cv.CV_AUTOSTEP)
        return mat

    def inpaint(src_np, msk_np, radius, flags):
        src_cv = cv.CreateMat(src_np.shape[0], src_np.shape[1], cv.CV_8UC3)
        msk_cv = cv.CreateMat(msk_np.shape[0], msk_np.shape[1], cv.CV_8UC1)
        dst_cv = cv.CreateMat(src_np.shape[0], src_np.shape[1], cv.CV_8UC3)
        cv.SetData(src_cv, src_np, cv.CV_AUTOSTEP)
        cv.SetData(msk_cv, msk_np, cv.CV_AUTOSTEP)
        cv.Inpaint(src_cv, msk_cv, dst_cv, radius, flags)
        dst = [ [ dst_cv[i,j] for j in xrange(src_np.shape[1]) ]
                              for i in xrange(src_np.shape[0]) ]
        return np.asarray(dst)[...,::-1] # BGR -> RGB

    INPAINT_NS = cv.CV_INPAINT_NS
    INPAINT_TELEA = cv.CV_INPAINT_TELEA

#
# Earth Mover's Distance
#
# @reference
#  Y. Rubner, C. Tomasi, and L. J. Guibas. A Metric for Distributions with
#  Applications to Image Databases. Proceedings of the 1998 IEEE International
#  Conference on Computer Vision, Bombay, India, January 1998, pp. 59-66.
#

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

#
# Inpainting based on the Fast Marching Method
#
# @reference
#  Alexandru Telea. An Image Inpainting Technique Based on the Fast Marching Method.
#  Journal of graphics tools 9.1, 2004, pp. 23-34
#

def Inpainter(src, mask, radius=5, flags=INPAINT_TELEA):
    src = img_as_ubyte(src)
    mask = img_as_ubyte(mask)
    dst = inpaint(src, mask, radius, flags)
    return dst

