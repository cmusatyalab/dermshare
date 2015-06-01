#
# Gemini -- symmetry based feature extraction
#
# Copyright (c) 2014-2015 Carnegie Mellon University
# All rights reserved.
#
# This software is distributed under the terms of the Eclipse Public
# License, Version 1.0 which can be found in the file named LICENSE.
# ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS SOFTWARE CONSTITUTES
# RECIPIENT'S ACCEPTANCE OF THIS AGREEMENT
#

import math
#import matplotlib.pyplot as plt
import numpy as np
from skimage.draw import polygon
from skimage.filter import roberts_negative_diagonal, roberts_positive_diagonal
from skimage.morphology import disk, binary_erosion, binary_dilation
from skimage.transform import rotate

from .emd import EarthMoversDistance
from .prst import PlanarReflectiveSymmetryTransform
from .util import Point, pad_for_rotation, sample_pixels, image_roi_offset

def find_axes(image_roi, attrs, debug=False):
    """ returns center, major and minor axes """
    center, major, minor = \
        PlanarReflectiveSymmetryTransform(image_roi, attrs=attrs, debug=debug)
    bbox, scale = image_roi_offset(attrs, image_roi.shape)

    if debug:
        print """\
Symmetry Detection Results
==========================
Center          : (%d, %d)
Major axis      : %d
Minor axis      : %d
Secondary axis  : %d
""" % (
        center.x / scale + bbox[1], center.y / scale + bbox[0],
        major.angle,
        (major+90).angle,
        minor.angle,
    )
    return center, major, minor


def _make_partitions(num_partitions, mask, center, axis):
    slice_, remainder = divmod(360, num_partitions)
    assert(remainder == 0)

    r = math.sqrt(sum(x**2 for x in mask.shape))

    for partition in xrange(num_partitions):
        start = int(axis.angle + partition * slice_)

        edge = [ Point(center.x + r * math.cos(math.radians(a)),
                       center.y + r * -math.sin(math.radians(a)))
                 for a in range(start, start + slice_ + 1) ]

        coords = np.array([center] + edge)

        rr, cc = polygon(coords[..., 1], coords[..., 0], shape=mask.shape)

        partition_map = np.zeros(mask.shape)
        partition_map[rr, cc] = mask[rr,cc]
        yield partition_map

def border_scores(image, center, major_axis, attrs={}, debug=False):
    eroded = binary_erosion(image[...,3], disk(4))
    border = eroded - binary_erosion(eroded, disk(1))
    border_mask = binary_dilation(border, disk(7))

    rn = roberts_negative_diagonal(image[...,0], border_mask)
    rp = roberts_positive_diagonal(image[...,0], border_mask)
    gn = roberts_negative_diagonal(image[...,1], border_mask)
    gp = roberts_positive_diagonal(image[...,1], border_mask)
    bn = roberts_negative_diagonal(image[...,2], border_mask)
    bp = roberts_positive_diagonal(image[...,2], border_mask)
    gradient = np.sqrt(rn**2 + rp**2 + gn**2 + gp**2 + bn**2 + bp**2)

    border_scores = [
        np.average(gradient, weights=section) * 255
        for section in _make_partitions(8, border_mask, center, major_axis)
        if section.sum() > 0
    ]

    attrs.update(('--Border Scores Section %d' % i, score)
                 for i, score in enumerate(border_scores, 1))
    attrs.update([
        ('Border Scores Average', np.mean(border_scores)),
        ('Border Scores Stddev', np.std(border_scores)),
    ])

    if debug:
        print "--- Border Scores ---"
        print " | ".join("%.3f" % score for score in border_scores)
        print

        plt.subplot(121)
        imgplot = plt.imshow((image[...,:3].T * border_mask.T).T)
        imgplot.set_interpolation('nearest')

        plt.subplot(122)
        imgplot = plt.imshow(gradient)
        imgplot.set_interpolation('nearest')
        plt.show()
    
    attrs['--Border Score'] = sum(score >= 10 for score in border_scores)
    return attrs 

def shape_symmetry(image, center, major_axis, attrs={}, debug=False):
    # pad to make image center coincide with symmetry center
    lesion_mask, _ = pad_for_rotation(image[..., 3], center)

    rotated = rotate(lesion_mask, 90-major_axis.angle)
    flipped = rotated[:,::-1]
    diff = np.logical_xor(rotated, flipped)

    pixels_diff = diff.sum() / 2.
    major_ratio = pixels_diff / rotated.sum()

    if debug:
        print """\
==== Shape Symmetry ====
--- Major Axis ---
num of pixels   : %d
shape sym ratio : %.3f
""" % (pixels_diff, major_ratio)

        plt.subplot(231)
        plt.imshow(rotated)
        plt.subplot(232)
        plt.imshow(flipped)
        plt.subplot(233)
        plt.imshow(diff)

    rotated = rotate(lesion_mask, 180-major_axis.angle)
    flipped = rotated[:,::-1]
    diff = np.logical_xor(rotated, flipped)

    pixels_diff = diff.sum() / 2.
    minor_ratio = pixels_diff / rotated.sum()

    if debug:
        print """\
--- Minor Axis ---
num of pixels   : %d
shape sym ratio : %.3f
""" % (pixels_diff, minor_ratio)

        plt.subplot(234)
        plt.imshow(rotated)
        plt.subplot(235)
        plt.imshow(flipped)
        plt.subplot(236)
        plt.imshow(diff)
        plt.show()

    attrs.update([
        ('Shape Asymmetry Major Ratio', major_ratio),
        ('Shape Asymmetry Minor Ratio', minor_ratio),
        ('--Shape Asymmetry Score', (major_ratio > 0.13)*1 + (minor_ratio > 0.15)*1),
    ])

def color_symmetry(image, center, major_axis, attrs={}, debug=False):
    signatures = []
    for mask in _make_partitions(2, image[...,3], center, major_axis):
        pixels = sample_pixels(image[...,:3], 250, mask)
        signatures.append(pixels * 255)
    major_emd = EarthMoversDistance(*signatures)
    
    signatures = []
    for mask in _make_partitions(2, image[...,3], center, major_axis + 90):
        pixels = sample_pixels(image[...,:3], 250, mask)
        signatures.append(pixels * 255)
    minor_emd = EarthMoversDistance(*signatures)

    if debug:
        print """\
==== Color Symmetry ====
Earth Mover's Distance (Major Axis) : %.3f
Earth Mover's Distance (Minor Axis) : %.3f
""" % (major_emd, minor_emd)

    attrs.update([
        ('Color Asymmetry Major EMD', major_emd),
        ('Color Asymmetry Minor EMD', minor_emd),
        ('--Color Asymmetry Score', (major_emd > 16)*1 + (minor_emd > 16)*1),
    ]) 

