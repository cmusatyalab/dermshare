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

# Segmenter

import numpy as np

from scipy.cluster.vq import kmeans, vq
from skimage.measure import label, regionprops
from skimage.morphology import square

from .image import rgb2labnorm, labnorm2rgb, morph_close, median_filter
from .opencv import EarthMoversDistance

RADIUS_IMPORTANCE_FACTOR = 2
EXCEPTION_FACTOR = 0.75
SMALL_REGION_THRESHOLD = 900
NUM_SUPER_CLUSTERS = 6

def _clusterKMeans(image, K, numIterations, debug=None):
    width, height = image.shape[:2]
    
    if True:
        ## add an extra channel containing distance from center to make
        ## clusters favor circular features around the center of the image

        # if we have a 2 pixel wide image, the center should be between pixels 0 and 1
        center_x, center_y = (width-1)/2., (height-1)/2.

        # compute distance from center
        dx = np.fromfunction(lambda x,y: x - center_x, (width,1))
        dy = np.fromfunction(lambda x,y: y - center_y, (1,height))
        dist_from_center = np.sqrt(np.add(np.square(dx), np.square(dy)))
        dist_from_center /= dist_from_center[0,0]

        # add as a new channel to image
        image = np.dstack([image, dist_from_center])

    # sample 10000 pixels and use K-means to determine the clusters
    channels = image.shape[2]
    pixels = image.reshape(-1, channels)
    npoints = pixels.shape[0]
    indices = np.random.choice(npoints, 10000)
    samples = pixels[indices]

    centroids, _ = kmeans(samples, K, numIterations)
    quantized, _ = vq(pixels, centroids)

    clustered = quantized.reshape(width, height)

    if debug:
        debug["clustered"] = labnorm2rgb(centroids[clustered])
    
    return centroids, clustered

def remove_small_regions(mask, threshold):
    labelled = label(mask, connectivity=2)
    small_regions = ( labelled == prop.label
                       for prop in regionprops(labelled)
                       if prop.area < threshold )
    for region in small_regions:
        mask = mask & ~region
    return mask

def clean_mask(image):
    mask = image != 0
    mask = remove_small_regions(mask, SMALL_REGION_THRESHOLD)
    mask = morph_close(mask)
    image = mask[...,np.newaxis] * 1.
    mask = median_filter(image, square(5)) == 0
    mask = remove_small_regions(mask, SMALL_REGION_THRESHOLD * 2) == 0
    return mask * 1.

def Segmenter(image, debug=None):
    width, height = image.shape[:2]
    image = image / 255.

    image = median_filter(image, square(3))
    image = rgb2labnorm(image)

    clusters, clustered = _clusterKMeans(image, 30, 40, debug)

    K = NUM_SUPER_CLUSTERS
    numIterations = 40

    clusters[...,3] *= RADIUS_IMPORTANCE_FACTOR
    superclusters, _ = kmeans(clusters, K, numIterations)

    # reassign clusters to super cluster values
    clustered = vq(clusters, superclusters)[0][clustered]

    superclusters[...,3] /= RADIUS_IMPORTANCE_FACTOR
    segmented_image = superclusters[clustered]

    # smooth edges using filterMedian 5x5
    filtered_segmentation = median_filter(segmented_image, square(5))

    # classify foreground vs background
    # - identify the (likely) camera aperture
    #camera_mask = (filtered_segmentation[...,3] > 0.8) * 1.

    # - exception mask
    #   check for all the clusters whether they are too far away from the center
    #   if it is over a pre-set threshold, consider that as mask
    #
    # (not sure why the camera mask couldn't be changed from .8 to .75 -JH)
    #EXCEPTION_FACTOR = 0.75
    #exception_mask = (filtered_segmentation[...,3] > EXCEPTION_FACTOR) * 1.

    # set up for finding skin vs. lesion
    skin_signature = None 
    lesion_mask = np.zeros(image.shape[:2], dtype=bool)

    # - order superclusters on distance from center, from outside to inside
    order = np.argsort(superclusters[...,3])[::-1]
    for N, idx in enumerate(order):
        if superclusters[idx][3] > 0.8:
            # camera mask
            continue

        # generate mask consisting of all pixels in this cluster 
        mask = (clustered == idx)

        # assume first cluster > 2000 pixels is representative for skin
        if skin_signature is None and np.sum(mask * 1.) <= 2000:
            continue

        # sample 200 pixels from each cluster for a signature
        y, x = np.where(mask)
        samples = np.random.choice(len(y), 200)
        signature = image[y[samples], x[samples]] * 255

        if skin_signature is None:
            skin_signature = signature
            continue

        # exception mask, these clusters should not be considered lesion
        if superclusters[idx][3] > EXCEPTION_FACTOR:
            continue

        # is this cluster similar enough to what we believe to be skin
        if EarthMoversDistance(skin_signature, signature) < 30:
           continue 

        # anything else must be lesion
        lesion_mask |= mask
        
    lesion_mask = clean_mask(lesion_mask) * 1.

    if debug:
        debug["segmented"] = labnorm2rgb(segmented_image)
        debug["segmFiltered"] = labnorm2rgb(filtered_segmentation)
        debug["segmFilled"] = lesion_mask
    
    return lesion_mask

