/*
 * DermShare Autosegmenter
 *
 * Copyright (c) 2010 University of Pittsburgh
 * Copyright (c) 2011-2015 Carnegie Mellon University
 * All rights reserved.
 *
 * This software is distributed under the terms of the Eclipse Public
 * License, Version 1.0 which can be found in the file named LICENSE.
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS SOFTWARE CONSTITUTES
 * RECIPIENT'S ACCEPTANCE OF THIS AGREEMENT
 */

#ifndef _SEGMENTER_ENUMS_H
#define _SEGMENTER_ENUMS_H

// color space
enum{
	COLORSPACE_NONE,
	COLORSPACE_RGB,
	COLORSPACE_CIE_LAB,
};


// using coordinates?
enum{
	COORDINATESYSTEM_NONE,
	COORDINATESYSTEM_POLAR,
};

#endif
