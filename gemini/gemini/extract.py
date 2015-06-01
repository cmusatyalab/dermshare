#
# Gemini -- image feature extraction
#
# Copyright (c) 2014-2015 Carnegie Mellon University
# All rights reserved.
#
# This software is distributed under the terms of the Eclipse Public
# License, Version 1.0 which can be found in the file named LICENSE.
# ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS SOFTWARE CONSTITUTES
# RECIPIENT'S ACCEPTANCE OF THIS AGREEMENT
#

import sys
sys.path.append('.')

import csv
import numpy as np
import os

from . import palette, symmetry, texture, zortea
from .util import read_image, crop_lesion, scale_lesion

def extract_features(image, attrs={}, debug=False):
    np.random.seed(0)

    image_roi = crop_lesion(image, attrs=attrs, debug=debug)

    palette.color_histogram(image_roi, attrs, debug)    # improves TP, worsens TN

    # symmetry detection uses uniformly scaled lesion mostly for performance
    scaled_roi = scale_lesion(image_roi, 100)
    center, major_axis, minor_axis = symmetry.find_axes(scaled_roi, attrs, debug)

    symmetry.border_scores(scaled_roi, center, major_axis, attrs, debug=debug)
                                                        # worsens TP, improves TN
    symmetry.shape_symmetry(scaled_roi, center, major_axis, attrs, debug=debug)
                                                        # worsens TP, improves TN
    symmetry.color_symmetry(scaled_roi, center, major_axis, attrs, debug=debug)

    zortea.color_shape_asymmetry(scaled_roi, attrs, debug)
    zortea.colors_full_lesion(scaled_roi, attrs, debug)
    zortea.colors_peripheral_vs_central(scaled_roi, attrs, debug)
    zortea.geometric(scaled_roi, attrs, debug)         # worsens TP and TN

    texture.delphi_lbp(scaled_roi, attrs, debug)     # worsens TN
    texture.delphi_scharr(scaled_roi, attrs, debug)

    return attrs

if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument('--debug', action="store_true")
    parser.add_argument('--verbose', action="store_true")
    parser.add_argument('--output')
    parser.add_argument('image', nargs='+')
    args = parser.parse_args()

    csvout = None
    
    for imagefile in args.image:
        print imagefile

        image = read_image(imagefile, size=(640,640), debug=args.debug)

        attrs = extract_features(image, debug=args.debug)
        features = dict((k, v) for k, v in attrs.iteritems()
                        if not k.startswith('--'))

        if args.verbose:
            print "==================\n==== features ====\n=================="
            for k in sorted(attrs.keys()):
                print "%-25s: %s" % (k.lower().replace(' ', '_'), attrs[k])
            print "\n%d features" % len(features)

        if args.output is None:
            outfile = os.path.splitext(imagefile)[0] + '.attrs'
            with open(outfile, 'wb') as f:
                writer = csv.writer(f)
                writer.writerows(features.items())
        else:
            if not csvout:
                output = open(args.output, 'wb')
                csvout = csv.DictWriter(output,
                                        ['image', 'diagnosis'] + sorted(features))
                csvout.writeheader() 

            features.update((
                ('image', os.path.basename(imagefile)),
                ('diagnosis', os.path.basename(os.path.dirname(imagefile))),
            ))
            csvout.writerow(features)

