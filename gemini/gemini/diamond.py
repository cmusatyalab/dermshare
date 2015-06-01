#
# Gemini -- Diamond filters
#
# Copyright (c) 2014-2015 Carnegie Mellon University
# All rights reserved.
#
# This software is distributed under the terms of the Eclipse Public
# License, Version 1.0 which can be found in the file named LICENSE.
# ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS SOFTWARE CONSTITUTES
# RECIPIENT'S ACCEPTANCE OF THIS AGREEMENT
#

from opendiamond.filter import Filter
from opendiamond.filter.parameters import BooleanParameter, NumberParameter, StringParameter

import cPickle as pickle
import math
import numpy as np
import os
from PIL import Image
import requests
from cStringIO import StringIO
from skimage.transform import rescale
import sys

from .extract import extract_features
from .trainingset import train_classifier, validate_classifier, compute_scaler

HEATMAP_NAME = '_filter.%s.heatmap.png'
ATTR_PREFIX = 'gemini.%s.'

# extract segmentation from the label data in Algum
# returns True
# sets HEATMAP_NAME.format(self.session.name) as heatmap
class SegmentationFilter(Filter):
    params = (
        BooleanParameter("red_channel"),
        BooleanParameter("green_channel"),
        BooleanParameter("blue_channel"),
        BooleanParameter("alpha_channel"),
    )
    def __call__(self, obj):
        try:
            label_image = self._get_label_image(obj)
        except:
            return False

        label = np.asarray(label_image)
        mask = np.zeros(label.shape[:2])
        if self.red_channel:
            mask = np.logical_or(mask, label[...,0]) * 1.
        if self.green_channel:
            mask = np.logical_or(mask, label[...,1]) * 1.
        if self.blue_channel:
            mask = np.logical_or(mask, label[...,2]) * 1.
        if self.alpha_channel and label.shape[2] == 4:
            mask = np.logical_or(mask, label[...,3]) * 1.

        segmentation = Image.fromarray(np.uint8(mask * 255.))
        obj.set_heatmap(HEATMAP_NAME % self.session.name, segmentation)
        return True

    def _get_label_image(self, obj):
        r = requests.get(obj['dermshare.segmentation-url'])
        r.raise_for_status()
        return Image.open(StringIO(r.content))


# extract segmentation from the blob argument
# returns True
# sets HEATMAP_NAME.format(self.session.name) as heatmap
class FixedSegmentationFilter(SegmentationFilter):
    def _get_label_image(self, _obj):
        return Image.open(StringIO(self.blob))

# extract features and classify object
# returns delphi (severity) score [0, 1]
# sets normalized features as attributes
class GeminiFilter(Filter):
    params = (
        StringParameter("segmenter"),
    )
    def __init__(self, *args, **kwargs):
        super(GeminiFilter, self).__init__(*args, **kwargs)
        self.CLASSIFIER, self.SCALER = build_classifier()
        self.attr_prefix = ATTR_PREFIX % self.session.name

    def __call__(self, obj):
        image = np.asarray(obj.image) / 255.
        heatmap = obj.get_heatmap(HEATMAP_NAME % self.segmenter)
        alpha = np.asarray(heatmap) / 255.

        # scale image and alpha to 640x640
        size = (640, 640)
        scale_x = float(size[0]) / image.shape[1]
        scale_y = float(size[1]) / image.shape[0]
        scale = min(scale_x, scale_y)

        # antialias for image data, nearest neighbor for alpha
        image = rescale(image, scale, order=3)
        alpha = rescale(alpha, scale, order=0)

        # combine into single image with alpha
        image = np.dstack((image, alpha)).copy(order='C')

        attrs = extract_features(image)

        feature_names, feature_values = \
            zip(*sorted((k, v) for k, v in attrs.iteritems()
                        if not k.startswith('--')))

        # compute delphi severity score
        proba = self.CLASSIFIER.predict_proba([feature_values])[0]
        severity = proba[0]

        # normalize features
        feature_values = list(self.SCALER.transform([feature_values])[0])

        # add delphi score as additional feature
        feature_names += ("Delphi Score",)
        feature_values.append(severity)

        # set features values as object attributes
        for feature, value in zip(feature_names, feature_values):
            name = feature.lower().replace(' ', '_')
            obj[self.attr_prefix + name] = value

        return severity

# compute euclidean distance between object and argument features
# returns distance
class DistanceFilter(Filter):
    params = (
        StringParameter("feature_extractor"),
        NumberParameter("border_scores_average"),
        NumberParameter("border_scores_stddev"),
        NumberParameter("color_asymmetry_major_emd"),
        NumberParameter("color_asymmetry_minor_emd"),
        NumberParameter("color_histogram_black"),
        NumberParameter("color_histogram_blue_gray"),
        NumberParameter("color_histogram_dark_brown"),
        NumberParameter("color_histogram_light_brown_tan"),
        NumberParameter("color_histogram_red"),
        NumberParameter("color_histogram_white_pink"),
        NumberParameter("color_shape_asymmetry_mean"),
        NumberParameter("color_shape_asymmetry_stddev"),
        NumberParameter("colors_histogram_average"),
        NumberParameter("colors_histogram_nonzero"),
        NumberParameter("colors_histogram_variance"),
        NumberParameter("colors_pvsc_density_baysian_l"),
        NumberParameter("colors_pvsc_density_baysian_a"),
        NumberParameter("colors_pvsc_density_baysian_b"),
        NumberParameter("colors_pvsc_mean_difference_l"),
        NumberParameter("colors_pvsc_mean_difference_a"),
        NumberParameter("colors_pvsc_mean_difference_b"),
        NumberParameter("geometric_p25"),
        NumberParameter("geometric_p50"),
        NumberParameter("geometric_p75"),
        NumberParameter("lesion_area"),
        NumberParameter("lesion_area_filled"),
        NumberParameter("lesion_eccentricity"),
        NumberParameter("lesion_formfactor"),
        NumberParameter("lesion_intensity_max"),
        NumberParameter("lesion_intensity_mean"),
        NumberParameter("lesion_intensity_min"),
        NumberParameter("lesion_major_axis_length"),
        NumberParameter("lesion_minor_axis_length"),
        NumberParameter("lesion_perimeter"),
        NumberParameter("lesion_solidity"),
        NumberParameter("local_binary_pattern_0"),
        NumberParameter("local_binary_pattern_1"),
        NumberParameter("local_binary_pattern_2"),
        NumberParameter("local_binary_pattern_3"),
        NumberParameter("local_binary_pattern_4"),
        NumberParameter("local_binary_pattern_5"),
        NumberParameter("local_binary_pattern_6"),
        NumberParameter("local_binary_pattern_7"),
        NumberParameter("local_binary_pattern_8"),
        NumberParameter("scharr_0"),
        NumberParameter("scharr_1"),
        NumberParameter("scharr_2"),
        NumberParameter("scharr_3"),
        NumberParameter("scharr_4"),
        NumberParameter("scharr_5"),
        NumberParameter("scharr_6"),
        NumberParameter("scharr_7"),
        NumberParameter("shape_asymmetry_major_ratio"),
        NumberParameter("shape_asymmetry_minor_ratio"),
        NumberParameter("delphi_score"),
    )

    def __init__(self, *args, **kwargs):
        super(DistanceFilter, self).__init__(*args, **kwargs)
        self.attr_prefix = ATTR_PREFIX % self.feature_extractor

        self.reference_values = []
        for feature in ( str(f) for f in self.params[1:] ):
            value = getattr(self, feature)
            if not math.isnan(value):
                self.reference_values.append((feature, value))

    def __call__(self, obj):
        distance = sum(pow(float(obj[self.attr_prefix + feature]) - value, 2)
                       for feature, value in self.reference_values)
        return math.sqrt(distance / len(self.reference_values))


# load/retrain classifier
def build_classifier():
    CACHED = os.path.join(sys.prefix, 'classifier.pickle')
    try:
        with open(CACHED, 'rb') as f:
            CLASSIFIER, SCALER = pickle.load(f)
    except:
        CLASSIFIER = train_classifier()
        validate_classifier(CLASSIFIER)
        SCALER = compute_scaler()
        with open(CACHED, 'wb') as f:
            pickle.dump((CLASSIFIER, SCALER), f)
    return CLASSIFIER, SCALER

def run_filters():
    Filter.run([SegmentationFilter, FixedSegmentationFilter, GeminiFilter,
            DistanceFilter])

