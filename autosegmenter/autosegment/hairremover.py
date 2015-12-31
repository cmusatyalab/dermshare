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

from skimage.color import rgb2luv

from .image import morph_close, morph_dilate
from .opencv import Inpainter

def HairRemover(image, debug=None):
    # =================================================================
    # extract hair as morphologically thin structures
    # -----------------------------------------------------------------

    # convert to Luv color space
    Luv_image = rgb2luv(image)
    L = Luv_image[..., 0]

    # a hard threshold is then applied to the difference between
    # the luminance before and after morphological closing
    # the dark pigmented elements have a large intensity in the
    # difference image
    LClose = morph_close(L)
    LDiff = LClose - L

    # Threshold to create mask for inpainting
    # set all pixels > 11.9 -> 255 and < 12 -> 0
    # dilate by 1 to remove boundaries
    threshold = 10.0    # original comment and code did not match... -JH

    # threshold operation is directly performed on LDiff
    mask = (morph_dilate(LDiff) >= threshold) * 1.

    result = Inpainter(image, mask, 5)

    if debug is not None:
        debug["inpaintingMask"] = mask
        debug["hairRemoved"] = result
    
    return result

