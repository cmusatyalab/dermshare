#
# Gemini -- color palette / histogram features
#
# Copyright (c) 2014-2015 Carnegie Mellon University
# All rights reserved.
#
# This software is distributed under the terms of the Eclipse Public
# License, Version 1.0 which can be found in the file named LICENSE.
# ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS SOFTWARE CONSTITUTES
# RECIPIENT'S ACCEPTANCE OF THIS AGREEMENT
#

import numpy as np
from skimage.segmentation import slic

# Color classes from dermfind ellipsoids
RGB_COLOR_CLASSES = [
    ('black',           (40, 40, 40), (40, 40, 40), (3, 2, 2), (-1, -1.5, 3)),
    ('dark brown',      (90, 50, 30), (50, 30, 30), (1, 1, 1), (-0.5, -0.5, 3)),
    ('light brown tan', (140, 100, 80), (50, 40, 30), (2, 3, 3), (-1.5, -1, 2)),
    ('red',             (100, 40, 30), (50, 30, 30), (4, 2, 3), (-3, 1.5, 3)),
    ('blue gray',       (80, 80, 110), (50, 30, 30), (1, 1, 1), (-0.5, -0.5, 1)),
    ('white pink',      (180, 160, 160), (40, 40, 40), (2, 3, 2), (-1.5, -1, 3)),
]
LAB_COLOR_CLASSES = [
    ('black',           (75, 125, 125), (50, 50, 50), (1, 0, 0), (0, 1, 0)),
    ('dark brown',      (80, 150, 180), (30, 30, 30), (1, 0, 0), (0, 1, 0)),
    ('dark brown',      (120, 175, 150), (30, 30, 30), (1, 0, 0), (0, 1, 0)),
    ('light brown tan', (150, 140, 150), (30, 30, 30), (1, 0, 0), (0, 1, 0)),
    ('light brown tan', (170, 150, 200), (30, 30, 30), (1, 0, 0), (0, 1, 0)),
    ('red',             (100, 175, 175), (30, 30, 30), (1, 0, 0), (0, 1, 0)),
    ('red',             (150, 175, 175), (30, 30, 30), (1, 0, 0), (0, 1, 0)),
    ('blue gray',       (150, 150, 75), (30, 30, 30), (1, 0, 0), (0, 1, 0)),
    ('blue gray',       (180, 100, 50), (30, 30, 30), (1, 0, 0), (0, 1, 0)),
    ('blue gray',       (125, 120, 100), (30, 30, 30), (1, 0, 0), (0, 1, 0)),
    ('blue gray',       (125, 150, 140), (30, 30, 30), (1, 0, 0), (0, 1, 0)),
    ('white pink',      (255, 140, 125), (30, 30, 30), (1, 0, 0), (0, 1, 0)),
    ('white pink',      (235, 120, 80), (30, 30, 30), (1, 0, 0), (0, 1, 0)),
]

def color_histogram(image_roi, attrs={}, debug=False):
    # segment image using kmeans clustering
    #segments = slic(image_roi[...,:3], sigma=1, n_segments=10, max_iter=30)
    segments = slic(image_roi[...,:3], sigma=1, n_segments=100, max_iter=10)
    nsegments = segments.max()

    CLASS_COLORS = np.array([ c[1] for c in RGB_COLOR_CLASSES ])/255.
    totals = [ 0. for c in RGB_COLOR_CLASSES ]

    lesion_mask = image_roi[...,3]

    for i in range(nsegments+1):
        area = np.logical_and(segments == i, lesion_mask)
        segment_size = area.sum()
        if segment_size == 0: continue

        rgb_average = image_roi[area,:3].mean(axis=0)

        delta = CLASS_COLORS - rgb_average
        distances = np.sqrt((delta * delta).sum(axis=1))
        closest = np.argmin(distances)

        totals[closest] += segment_size

    totals = np.array(totals) / sum(totals)

    for i, v in enumerate(totals):
        name = RGB_COLOR_CLASSES[i][0]
        attrs["Color Histogram %s" % name] = v

