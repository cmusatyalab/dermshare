#
# Gemini -- Planar-Reflective Symmetry Transform
#
# Copyright (c) 2014-2015 Carnegie Mellon University
# All rights reserved.
#
# This software is distributed under the terms of the Eclipse Public
# License, Version 1.0 which can be found in the file named LICENSE.
# ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS SOFTWARE CONSTITUTES
# RECIPIENT'S ACCEPTANCE OF THIS AGREEMENT
#
# @reference
#  Joshua Podolak, Philip Shilane, Aleksey Golovinskiy, Szymon Rusinkiewicz,
#  and Thomas Funkhouser. A Planar-Reflective Symmetry Transform for 3D Shapes.
#  ACM Transactions on Graphics (Proc. SIGGRAPH). 25(3) July 2006
#

#import matplotlib.pyplot as plt
import numpy as np
from skimage.feature import peak_local_max
from skimage.filter import gaussian_filter
from skimage.transform import rotate

from .util import Point, Axis, pad_for_rotation, image_roi_offset, draw_axes

def _symToXY(axis, image_center=None):
    """ convert from symmetrymap coordinates to roi coordinates """
    if image_center is None: image_center = Point(0,0)

    X = (axis.radius *  axis.sin) / 2 + image_center.x
    Y = (axis.radius * -axis.cos) / 2 + image_center.y
    return Point(X, Y)

def _find_intersection(axis1, axis2, image_center=None):
    """ estimate symmetry center from the major and minor axes.
    takes into account that the image's vertical axis points down
    """
    if image_center is None: image_center = Point(0,0)

    dt = axis1.cos * axis2.sin - axis1.sin * axis2.cos
    assert(dt != 0) # axes must not be parallel!

    x = (axis2.cos * axis1.radius - axis1.cos * axis2.radius) / dt + image_center.x
    y = (axis1.sin * axis2.radius - axis2.sin * axis1.radius) / dt + image_center.y
    return Point(x, y)

def PlanarReflectiveSymmetryTransform(image_roi, anglestep=1, attrs={}, debug=False):
    """ Planar-Reflective Symmetry Transform """
    roi, image_center = pad_for_rotation(image_roi[...,3])
    size = roi.shape[0]

    ############################################
    # apply Planar-Reflective Symmetry Transform
    angles, remainder = divmod(180, anglestep)
    assert(remainder == 0)

    num_offsets = 2 * size - 1
    symmetryMap = np.zeros((angles, num_offsets))

    # for each angle, rotate the image, and compare with reflection
    for angle in range(0, angles):
        imgR = rotate(roi, -angle * anglestep)
        imgF = imgR[::-1]

        for offset in range(size):
            symmetryMap[angle, offset] = \
               (imgR[:offset] * imgF[(size-offset):]).sum()

        for offset in range(size-1):
            symmetryMap[angle, size+offset] = \
               (imgR[offset:] * imgF[:(size-offset)]).sum()

    symmetryMap = gaussian_filter(symmetryMap, 5.)
    ############################################

    ############################################
    # find major and minor symmetry axes in symmetry map
    anglestep = 180 / symmetryMap.shape[0]
    size = (symmetryMap.shape[1] + 1) / 2

    # find axes by looking for local maxima
    symScore = np.max(symmetryMap)
    maxima = peak_local_max(symmetryMap, threshold_abs=symScore*.2,
        min_distance=30, num_peaks=5000, exclude_border=False)

    peak_scores = symmetryMap[maxima.T[0], maxima.T[1]]
    order = np.argsort(peak_scores)[::-1]

    idxMajor = order[0]
    angle, offset = maxima[idxMajor]
    major_axis = Axis(
        angle  = angle * anglestep,
        radius = size - offset,
        score  = peak_scores[idxMajor],
    )

    # find a local maximum closest to perpendicular to the major axis
    idxMinor = None
    minAngleDiff = 180
    threshold = symScore*.6
    perpendicular = major_axis + 90

    for idx in order[1:]:
        # if we found a minor axis, skip scores below threshold
        if idxMinor is not None and peak_scores[idx] <= threshold:
            break

        angle, _ = maxima[idx]

        angleDiff = 90 - abs(abs((angle * anglestep) - perpendicular.angle) - 90)
        if angleDiff > minAngleDiff: continue

        minAngleDiff = angleDiff
        idxMinor = idx

    if idxMinor is None:
        idxMinor = order[1]

    angle, offset = maxima[idxMinor]
    minor_axis = Axis(
        angle  = angle * anglestep,
        radius = size - offset,
        score  = peak_scores[idxMinor],
    )
    center = _find_intersection(major_axis, minor_axis, image_center)

    bbox, scale = image_roi_offset(attrs, image_roi.shape)
    attrs.update([
        #('Border Symmetry Angle', major_axis.angle),
        ('--Border Symmetry Score', major_axis.score),
        ('--Border Symmetry Axis Major', major_axis.angle),
        ('--Border Symmetry Axis Minor', perpendicular.angle),
        ('--Border Symmetry Axis Secondary', minor_axis.angle),
        ('--Border Symmetry Center X', center.x / scale + bbox[1]),
        ('--Border Symmetry Center Y', center.y / scale + bbox[0]),
    ])

    if debug:
        print "symCenterMajor", _symToXY(major_axis, image_center)
        print "symCenterMinor", _symToXY(minor_axis, image_center)
        print "symCenter     ", center

        plt.subplot(221)
        imgplot = plt.imshow(image_roi)
        imgplot.set_interpolation('nearest')
        plt.subplot(222)
        plt.imshow(symmetryMap)
        plt.subplot(223)
        imgplot = plt.imshow(draw_axes(image_roi[...,3]*255, center, major_axis))
        imgplot.set_interpolation('nearest')
        plt.subplot(224)
        imgplot = plt.imshow(draw_axes(image_roi[...,3]*255, center, minor_axis))
        imgplot.set_interpolation('nearest')
        plt.show()

    return center, major_axis, minor_axis

