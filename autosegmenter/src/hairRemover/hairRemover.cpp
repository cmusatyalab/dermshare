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

#include <iostream>
#include <fstream>

#include <vector>
#include <algorithm>

#include <glib.h>

#include "diamond/diamond.h"
#include "hairRemover.h"


using namespace std;


void CHairRemover::run()
{
	assert(_isValid);

	// ============================================================
	// extract hair as morphologically thin structures
	// ------------------------------------------------------------
	CIPPImage<Ipp32f> LDiff = extractThinStructureMorph(_image8u);

	// threshold the value to produce the mask for inpainting
	// set all pixels > 11.9 -> 255 and < 12 -> 0
	// threshold operation is directly performed on LDiff1
	
	float threshold = 10.f;
	_mask8u = getMask(LDiff, threshold);

	CInpainter ip(_image8u, _mask8u, 5);
	ip.run();
	//ip.run(CInpainter::INPAINTER_CRIMINISI);
	_result8u = ip.getResult();

	_isProcessed = true;
}


void CHairRemover::save(ImageMap &images, CDiamondContext &diamond)
{
	assert(_isProcessed);

	images["inpaintingMask"] = _mask8u;
	images["hairRemoved"] = _result8u;
}

CIPPImage<Ipp32f> CHairRemover::extractThinStructureMorph(const CIPPImage<Ipp8u> &image8u) const
{
	//CIPPImage<Ipp8u>::getLibInfo();
	// convert the 8u image to 32f for further process
	CIPPImage<Ipp32f> image32f = convert_8u32f(image8u)/255.;
	// color space transform, from RGB to Luv
	CIPPImage<Ipp32f> imageLUV = image32f.RGBToLUV();
	IppiSize size = image32f._size;

	// allocate memory for all three channels.
	CIPPImage<Ipp32f> L(size, 1);
	CIPPImage<Ipp32f> LClose(size, 1);

	L = imageLUV.getChannel(0);
	//IppStatus status = ippiCopy_32f_C3C1R(imageLUV._imageData, size.width*3*4, L._imageData, size.width*4, size);

	//for(int i = 0; i < L._size.width * L._size.height; i++)
	//{
	//	cout<< L._imageData[i] << ", "; 
	//}

	// perform morphological close operation (using spherical structuring element)
	// on the L(luminance) component of the Luv image 
	
	//se = strel('ball', 5, 3);
	//im_L_close = imclose(im_L, se);

	Ipp8u pMask[11*11] = {0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0,
						0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
						1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
						1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
						1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
						1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
						1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
						1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
						1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
						0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
						0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0};

	//Ipp8u pMask[9*9] = { 0, 0, 1, 1, 1, 1, 1, 0, 0,
	//					0, 1, 2, 2, 2, 2, 2, 1, 0,
	//					1, 2, 2, 3, 3, 3, 2, 2, 1,
	//					1, 2, 3, 3, 4, 3, 3, 2, 1,
	//					1, 2, 3, 4, 4, 4, 3, 2, 1,
	//					1, 2, 3, 3, 4, 3, 3, 2, 1,
	//					1, 2, 2, 3, 3, 3, 2, 2, 1,
	//					0, 1, 2, 2, 2, 2, 2, 1, 0,
	//					0, 0, 1, 1, 1, 1, 1, 0, 0};

	IppiPoint anchor = {5, 5};
	IppiSize maskSize = {11, 11};

	// -------------------------------------------------------------------------------------
	//IppiMorphAdvState* pState = NULL;
	//// allocate and initialize the morphAdvState
	//status = ippiMorphAdvInitAlloc_32f_C1R(&pState, L._size, pMask, maskSize, anchor);
	//status = ippiMorphCloseBorder_32f_C1R(L._imageData, size.width*4, LClose._imageData, 
	//										size.width*4, size, ippBorderRepl, pState);
	//ippiMorphAdvFree(pState);
	//pState = NULL;
	// --------------------------------------------------------------------------------------
	LClose = L.morphCloseBorder(pMask, maskSize, anchor);


	// a hard threshold is then applied to the difference between
	// the luminance before and after morphological closing
	// the dark pigmented elements have a large intensity in the difference
	// image
	//im_L_diff = im_L_close - im_L;
	CIPPImage<Ipp32f> LDiff = LClose - L;

	return LDiff;
}

CIPPImage<Ipp8u> CHairRemover::getMask(const CIPPImage<Ipp32f> &image32f, float threshold) const
{
	IppiSize size = image32f._size;
	CIPPImage<Ipp32f> LDiff1(size, 1);

	// dilate by 1 to remove boundaries
	// se = strel('diamond', 1);
	// im_L_diff = imdilate(im_L_diff, se);
	// ind = find(im_L_diff  >= threshold);
	Ipp8u pMask1[3*3] = {0, 1, 0,
						  1, 1, 1,
						  0, 1, 0};
	IppiSize maskSize1 = {3, 3};
	IppiPoint anchor1 = {1, 1};

	IppiMorphState* pState1 = NULL;

	IppStatus status = ippiMorphologyInitAlloc_32f_C1R( size.width*4, pMask1, maskSize1, anchor1, &pState1);
	if (status)
		displayStatus(status);
	status = ippiDilateBorderReplicate_32f_C1R( image32f._imageData, size.width*4, LDiff1._imageData, size.width*4, size, ippBorderRepl, pState1);
	if (status)
		displayStatus(status);
	ippiMorphologyFree(pState1);
	pState1 = NULL;

	status = ippiThreshold_LTVal_32f_C1IR(LDiff1._imageData, size.width*4, size, threshold, 0);
	if (status)
		displayStatus(status);
	status = ippiThreshold_GTVal_32f_C1IR(LDiff1._imageData, size.width*4, size, threshold-0.01, 255);
	if (status)
		displayStatus(status);
	return convert_32f8u(LDiff1);
}
