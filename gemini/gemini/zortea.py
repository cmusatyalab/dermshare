# -*- coding: utf-8 -*-
#
# Gemini -- Features as described by M. Zortea et al
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
#  Maciel Zortea, Thomas R. Schopf, Kevin Thon, Marc Geilhufe, Kristian
#  Hindberg, Herbert Kirchesch, Kajsa Møllersen, Jőrn Schulz, Stein Olav
#  Skrøvseth, Fred Godtliebsen. Performance of a demoscopy-based computer
#  vision system for the diagnosis of pigmented skin lesions compared with
#  visual evaluation by experienceѕ dermatologists. Artificial Intelligence in
#  Medicine 60 (2014) 13-26
#

import math
#import matplotlib.pyplot as plt
import numpy as np
from skimage.color import rgb2gray, rgb2lab
from skimage.measure import regionprops, label
from skimage.morphology import binary_erosion, binary_opening, disk

from .util import sample_pixels, pad_for_rotation

def color_shape_asymmetry(image_roi, attrs={}, debug=False):
    """ Asymmetry of color-shape """
    grayscale = rgb2gray(image_roi)
    lesion_mask = image_roi[..., 3]

    labels = label(lesion_mask, background=0)+1
    props = regionprops(labels)

    sizes = [ (p['area'], i) for i, p in enumerate(props) ]
    max_value, max_index = max(sizes)
    centroid = props[max_index]['centroid']

    masked = np.ma.masked_array(grayscale, 1-lesion_mask)
    tmin, tmax = masked.min(), masked.max()

    centroids = []
    for i in xrange(1, 10):
        threshold = (i / 10.) * (tmax - tmin) + tmin
        binary_mask = masked <= threshold

        labels = label(binary_mask, background=0)+1
        props = regionprops(labels)

        sizes = [ (p['area'], i) for i, p in enumerate(props) ]
        max_value, max_index = max(sizes)
        p = props[max_index]

        centroids.append(p['centroid'])

    distances = [ math.sqrt((x - centroid[1])**2 + (y - centroid[0])**2)
                  for y, x in centroids ]
    mean = np.mean(distances)
    std = np.std(distances)

    if debug:
        print """\
=== Asymmetry of Color-Shape ===
center of mass center mean: %.3f
center of mass stddev: %.3f
""" % (mean, std)

    attrs.update((
        ("Color shape asymmetry mean", mean),
        ("Color shape asymmetry stddev", std),
    ))


def colors_full_lesion(image_roi, attrs={}, debug=False):
    """ Colors, full lesion """
    image_lab = rgb2lab(image_roi[..., :3])
    lesion_mask = image_roi[..., 3]

    samples = sample_pixels(image_lab, 10000, lesion_mask)
    bins = (
        np.arange(0, 101, 2),
        np.arange(-127, 128, 2),
        np.arange(-127, 128, 2)
    )
    H, edges = np.histogramdd(samples, bins)

    nonzero_bins = H[np.nonzero(H)]

    attrs.update([
        ("Colors Histogram Average", np.mean(nonzero_bins)),
        ("Colors Histogram Variance", np.var(nonzero_bins)),
        ("Colors Histogram Nonzero", nonzero_bins.size / float(H.size)),
    ])

def colors_peripheral_vs_central(image_roi, attrs={}, debug=False):
    image_roi, center = pad_for_rotation(image_roi)
    lesion_mask = image_roi[..., 3]

    goal = lesion_mask.sum() * 0.7
    inner = lesion_mask.copy()
    while inner.sum() > goal:
        inner = binary_erosion(inner, disk(1))
    outer = np.logical_and(lesion_mask, np.logical_not(inner))

    if debug:
        print """\
=== Colors Peripheral vs Central ===
lesion area: %d
inner goal: %d
inner area: %d
outer area: %d
""" % (lesion_mask.sum(), goal, inner.sum(), outer.sum())

    if debug:
        plt.subplot(131)
        plt.imshow(lesion_mask)
        plt.subplot(132)
        plt.imshow(inner)
        plt.subplot(133)
        plt.imshow(outer)
        plt.show()

    outer = np.nonzero(outer)
    inner = np.nonzero(inner)

    image_lab = rgb2lab(image_roi[..., :3])
    L, a, b = np.dsplit(image_lab, 3)

    delta_L = np.mean(L[outer]) - np.mean(L[inner])
    delta_a = np.mean(a[outer]) - np.mean(a[inner])
    delta_b = np.mean(b[outer]) - np.mean(b[inner])

    density_L = (
        np.histogram(L[outer], 100, (0.,100.), density=True)[0] *
        np.histogram(L[inner], 100, (0.,100.), density=True)[0]
    ).sum()
    density_a = (
        np.histogram(a[outer], 254, (-127.,127.), density=True)[0] *
        np.histogram(a[inner], 254, (-127.,127.), density=True)[0]
    ).sum()
    density_b = (
        np.histogram(b[outer], 254, (-127.,127.), density=True)[0] *
        np.histogram(b[inner], 254, (-127.,127.), density=True)[0]
    ).sum()

    attrs.update([
        ('Colors PvsC mean difference L', delta_L),
        ('Colors PvsC mean difference a', delta_a),
        ('Colors PvsC mean difference b', delta_b),
        ('Colors PvsC density baysian L', density_L),
        ('Colors PvsC density baysian a', density_a),
        ('Colors PvsC density baysian b', density_b),
    ])


def geometric(image_roi, attrs={}, debug=False):
    """ Geometric """
    image_roi, _ = pad_for_rotation(image_roi)
    grayscale = rgb2gray(image_roi)
    lesion_mask = image_roi[..., 3]

    labels = label(lesion_mask, background=0)+1
    props = regionprops(labels)

    sizes = [ (p['area'], i) for i, p in enumerate(props) ]
    max_value, max_index = max(sizes)

    masked = np.ma.masked_array(grayscale, 1-lesion_mask)
    tmin, tptp = masked.min(), masked.ptp()

    # percentages reversed, lesion is darkest in the center
    p25 = tptp * .75 + tmin
    p50 = tptp * .50 + tmin
    p75 = tptp * .25 + tmin

    if debug:
        print "=== Geometric ==="

        plt.subplot(141)
        plt.imshow(labels.copy())

    for i, threshold in enumerate([p25, p50, p75], 1):
        pieces = masked < threshold

        pieces = binary_opening(pieces, disk(1))
        labels = label(pieces, background=0)+1
        N = labels.max()
        
        attrs['Geometric p%d' % (25*i)] = N

        if debug:
            print "%dth percentile : %d" % (25*i, N)
            plt.subplot(141 + i)
            plt.imshow(labels)

    if debug:
        plt.show()

