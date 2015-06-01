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
#include <cstdio>
#include "inpainter.h"

using namespace std;

#ifndef SAVE_INTERMEDIATE
#define SAVE_INTERMEDIATE 0
#endif

void getGradientSimple(const CIPPImage<Ipp8u> &image, CIPPImage<Ipp16s> &Dx, CIPPImage<Ipp16s> &Dy);

void CInpainter::run(InpainterType type)
{	
	switch(type)
	{
	case INPAINTER_CRIMINISI:
		runCriminisi();
		break;
	case INPAINTER_NS:
		runInpaintIPP(IPP_INPAINT_NS);
		break;
	case INPAINTER_TELEA:
	default:
		runInpaintIPP(IPP_INPAINT_TELEA);
	}
}
// ================================================================================================
// 
// IPP_INPAINT_TELEA
// [Telea04] A.Telea. An Image Inprinting Technique Based on
// the Fast Marching Method. Journal of Graphic Tools,
// vol.9, No.1, ACM Press, 2004.
//
//
// IPP_INPAINT_NS
// [Bert01] M.Bertalmio, A.L.Bertozzi, G.Sapiro. Navier-Stokes,
// Fluid Dynamics, and Image and Video Inpainting. Proc.
// ICCV 2001, pp.1335-1362, 2001.
//
// ------------------------------------------------------------------------------------------------
void CInpainter::runInpaintIPP(int flag = IPP_INPAINT_TELEA)
{
	// ============================================================================================
	// Distance transform using fast marching.
	int pBufferSize;
	CIPPImage<Ipp32f> distanceTransform(_size, 1, Ipp32f(0));

	IppStatus status = ippiFastMarchingGetBufferSize_8u32f_C1R(_size, &pBufferSize);
	if (status)
		displayStatus(status);
	Ipp8u *pBuffer = new Ipp8u[pBufferSize];
	status = ippiFastMarching_8u32f_C1R(_mask8u._imageData, _size.width, distanceTransform._imageData, _size.width*4, _size, 1, pBuffer);
	if (status)
		displayStatus(status);
	delete [] pBuffer;
	pBuffer = NULL;

	// -----------------------------------------------------------------------------------------------
	// flags Specifies algorithm for image inpainting; following values are possible:
	// IPP_INPAINT_TELEA Telea algorithm;
	// IPP_INPAINT_NS Navier-Stokes equation.

	IppiInpaintState_8u_C1R* pStateInpainting = NULL;

	// fill in the missing pixels with inpainting
	status = ippiInpaintInitAlloc_8u_C3R(&pStateInpainting, distanceTransform._imageData, _size.width*4, _mask8u._imageData, _size.width, _size, 1, IppiInpaintFlag(flag));
	if (status)
		displayStatus(status);
	status = ippiInpaint_8u_C3R(_imageOriginal._imageData, _size.width*3, _image8u._imageData, _size.width*3, _size, pStateInpainting);
	if (status)
		displayStatus(status);
	status = ippiInpaintFree_8u_C3R(pStateInpainting);
	if (status)
		displayStatus(status);
	pStateInpainting = NULL;
	// ==============================================================================================
}

// ===============================================================================================
// The following section of code implements the following paper.
// 
// A. Criminisi, P. P¨¦rez, and K. Toyama. 
// Object removal by exemplar-based inpainting. Conf. on Computer Vision and Pattern Recognition, 
// CVPR, Madison, Wisconsin, 2003.
// http://research.microsoft.com/vision/cambridge/i3l/patchworks.htm
// ------------------------------------------------------------------------------------------------
void CInpainter::runCriminisi(void)
{
	IppiPoint start = {0, 0};

	// ================================================================
	// prepare for all the data structures
	_imageGray		= _image8u.RGBToGray();
	_sourceMap		= CIPPImage<Ipp8u>(_size, 1, Ipp8u(0));
	_priorityMap	= CIPPImage<Ipp32f>(_size, 1, Ipp32f(0));
	_confidenceMap	= CIPPImage<Ipp32f>(_size, 1, Ipp32f(0));

	setBoundary(start, _size);// set the boundary for mask
	initConfidenceMap();	// initialize the confidence map;
	initSourceMap();// initialize the source map

	// initialize the gradient and normal map
	_Dx = getGradientSameSize(_imageGray, IPPIMAGE_HORIZONTAL);
	_Dy = getGradientSameSize(_imageGray, IPPIMAGE_VERTICAL);
	// ------------------------------------------------------------------------
	// 2. normal of the propagating front is the normalized gradient of the mask
	// ------------------------------------------------------------------------
	_Nx = getGradientSameSize(_mask8u, IPPIMAGE_HORIZONTAL);
	_Ny = getGradientSameSize(_mask8u, IPPIMAGE_VERTICAL);
	//_Dx = CIPPImage<Ipp16s>(_size, 1);
	//_Dy = CIPPImage<Ipp16s>(_size, 1);
	//_Nx = CIPPImage<Ipp16s>(_size, 1);
	//_Ny = CIPPImage<Ipp16s>(_size, 1);
	//getGradientSimple(_imageGray, _Dx, _Dy);
	//getGradientSimple(_mask8u, _Nx, _Ny);

	// calculate the priority score for all the boundary pixels
	setPriorityMap(start, _size);

	int count = 0;
	int xp, yp, xs, ys;	
	// if there are still non source pixels left, keep looping
	while(INPAINTER_SOURCE != _mask8u.getMax(&xp, &yp)) // here, xp and yp are used as dummy variables;
	{
		count++;
		if(0 == count % 10) cout << "." << flush;
		if(0 == count % 400) cout << endl << flush;

		findMaxPriority(xp, yp);

		// find the best source patch to match
		// inpaint the current patch with the source patch
		findBestPatch(xp, yp, xs, ys);

		// update the confidence, boundary and priority
		update(xp, yp, xs, ys);

		// save intermediate result.
#if SAVE_INTERMEDIATE
		char filename[255];
		sprintf(filename, "images/0000/%04d.jpg", count);
		_image8u.save(filename);
#endif
	}
}


bool CInpainter::load(const char* filename, const char* maskFilename)
{
	assert(filename);
	IppStatus status = _imageOriginal.load(filename, 0, 1);
	if(status != ippStsNoErr)
	{
		cout << "\nFATAL ERROR (CInpainter::load) -- loading image";
		_isValid = false;
		return false;
	}

	_size = _imageOriginal._size;

	// maskFilename has a default value of NULL
	// if a filename is not supplied, we assume that the 
	// green color [0 255 0] in the original image is masked
	// we need to make a mask
	if(NULL == maskFilename)
		_maskOriginal= makeMask(_imageOriginal, CVector3<int>(0, 255, 0));
	else
	{
		status = _maskOriginal.load(maskFilename, 0, 0);
	
		if(status != ippStsNoErr 
			|| _size.width != _maskOriginal._size.width
			|| _size.height != _maskOriginal._size.height)
		{
			cout << "\nFATAL ERROR (CInpainter::load) -- loading mask";
			_isValid = false;
			return false;
		}
	}
	
	// initialize the intermediate maps
	_image8u		= _imageOriginal;
	_mask8u			= _maskOriginal;
	_isValid = true;
	return true;
}



float CInpainter::getData(int x0, int y0) const
{
	long p, p0;
	int dxMax, dyMax,  magnitudeMax=0, dx, dy, magnitude;
	int x, y, height = _size.height, width = _size.width;

	// ------------------------------------------------------------------------
	// 1. graylevel gradient of the image
	// ------------------------------------------------------------------------
	for(y=max(y0-_sizeWindow.height,0);y<=min(y0+_sizeWindow.height,height-1);y++)
	for(x=max(x0-_sizeWindow.width,0);x<=min(x0+_sizeWindow.width,width-1);x++)
	{	
		// if one of p is a boundary point, discard
		if(x < 1 || x >= width-1 || y < 1 || y >= height - 1)
			continue;

		p = y * width + x;
		// find the greatest gradient in this patch, this will be the gradient of this pixel(according to "detail paper")
		if(_mask8u._imageData[p] == INPAINTER_SOURCE) // source pixel
		{
			//make sure this four neighbors do not touch target region(big jump in gradient)
			if(  _mask8u._imageData[p+1		] != INPAINTER_SOURCE
			   ||_mask8u._imageData[p-1		] != INPAINTER_SOURCE
			   ||_mask8u._imageData[p+width	] != INPAINTER_SOURCE
			   ||_mask8u._imageData[p-width	] != INPAINTER_SOURCE)
			   continue;
			
			dx = _Dx._imageData[p], dy = _Dy._imageData[p], magnitude = dx*dx+dy*dy;
			if(magnitude >= magnitudeMax)
				dxMax = dx, dyMax = dy, magnitudeMax = magnitude;
		}
	}

	// ------------------------------------------------------------------------
	// 2. normal of the propagating front is the normalized gradient of the mask
	// ------------------------------------------------------------------------
	p0 = y0 * width + x0;
	int Nx = -_Nx._imageData[p0];
	int Ny = -_Ny._imageData[p0];
	float magn = sqrt(float(Nx*Nx + Ny*Ny));
	
	if(magn == 0)
		return 0;
	// perpendicular to the gradient: (x,y)->(y, -x)
	return fabs(((Nx/magn)*(dyMax)+(Ny/magn)*(-dxMax))/255.f); // dot product
}

// HOWARD : TODO
// convert to non-brute force one
// convert image to LUV and search for best patch
void CInpainter::findBestPatch(int x0, int y0, int &xp, int &yp)
{
	int xt, yt;
	int heightW = _sizeWindow.height, widthW =_sizeWindow.width;

	// ======================================================
	// SSD using FFT
	// ------------------------------------------------------
	// extract the patch
	// copy the best patch from source to the image
	IppiSize roiSize = {widthW*2+1, heightW*2+1};
	xt = max(x0-widthW, 0);
	yt = max(y0-heightW, 0);
	int widtht = min(_size.width-xt, roiSize.width);
	int heightt = min(_size.height-yt, roiSize.height);
	IppiPoint startt = {xt, yt};
	//IppiPoint origin = {0, 0};
	IppiSize roiSizet = {widtht, heightt};

	// update both the mask and the image
	makeMaskedImage();

	CIPPImage<Ipp32f> patchToBeFilled = _imageMasked32f.getRegion(startt, roiSizet);
	CIPPImage<Ipp8u> patchMask = _maskInverted8u.getRegion(startt, roiSizet);
	//CIPPImage<Ipp8u> patchMask(roiSizet, 1);
	//patchToBeFilled.copyRegion(origin, _imageMasked32f, startt, roiSizet);
	//patchMask.copyRegion(origin, _maskInverted8u, startt, roiSizet);

	//std::cout <<"find1" << endl << flush;

	CIPPImage<Ipp32f> SSDResult = squaredDifferenceMasked_FFT(_imageMasked32f, patchToBeFilled, patchMask);
	//std::cout <<"find2" << endl << flush;

	SSDResult.getMin(&xp, &yp, _sourceMap);

	//std::cout <<"find3" << endl << flush;


	//// ======================================================
	//// brute force SSD calculation
	//// ------------------------------------------------------
	//int r, g, b, xs, ys;
	//int height=_size.height, width=_size.width;
	//long sumMin = 3*255*255*(2*heightW+1)*(2*widthW+1), sum, p, pt, ps;
	//
	//for(int y = heightW; y < height-heightW; y++)
	//for(int x = widthW; x < width-widthW; x++)
	//{	
	//	// first find a candidate patch
	//	p = y * width + x;
	//	// not a source patch, than go to next point
	//	if(!_sourceMap._imageData[p])
	//		continue;

	//	// then compare with the target patch
	//	sum=0;
	//	for(int y1=-heightW; y1 <= heightW; y1++)
	//	for(int x1=-widthW; x1 <= widthW; x1++)
	//	{
	//		xt = x0+x1, yt = y0+y1;
	//		xs = x+x1, ys = y+y1;
	//		
	//		// if out of bounds, do nothing
	//		if(xt < 0 ||xt > width-1 || yt < 0 || yt > height-1) continue;

	//		// add to total SSD only if the current point is a source 
	//		if(INPAINTER_SOURCE == _mask8u._imageData[yt * width + xt])
	//		{
	//			pt = (yt*width+xt)*3;
	//			ps = (ys*width+xs)*3;
	//			r = _image8u._imageData[pt  ] - _image8u._imageData[ps  ];
	//			g = _image8u._imageData[pt+1] - _image8u._imageData[ps+1];
	//			b = _image8u._imageData[pt+2] - _image8u._imageData[ps+2];
	//			sum+= r*r + g*g + b*b;
	//		}
	//	}
	//	if(sum<sumMin)
	//		sumMin=sum, xp = x, yp = y;
	//}

}

void CInpainter::update(int x0, int y0, int xp, int yp)
{
	int heightW = _sizeWindow.height, widthW =_sizeWindow.width;
	// copy the best patch from source to the image
	IppiSize roiSize = {widthW*2+1, heightW*2+1};
	int xt = max(x0-widthW, 0);
	int yt = max(y0-heightW, 0);
	xp = xp - min(x0-widthW, 0);
	yp = yp - min(y0-heightW, 0);
	int widtht = min(_size.width-xt, roiSize.width);
	int heightt = min(_size.height-yt, roiSize.height);
	IppiPoint startt = {xt, yt};
	IppiPoint starts = {xp-widthW, yp-heightW};
	IppiSize roiSizet = {widtht, heightt};

	// copy the image contents to the target region
	_image8u.copyRegionMasked(startt, _imageOriginal, starts, roiSizet, _mask8u, startt);
	
	// assign the confidence of the target pixel to all the new pixels that just got filled
	// in the target region
	// MUST DO THIS BEFORE WE UPDATE THE MASK
	_confidenceMap.setRegionMasked(getConfidence(x0, y0), startt, roiSizet, _mask8u, startt);	

	// update the mask
	_mask8u.setRegion(INPAINTER_SOURCE, startt, roiSizet);
	// update maskInverted8u


	// ===================================================================
	// update the region of interest
	// 
	// update boundary, gradient, normal, confidence, priority of a local region
	// that is affected by the copy operation
	// 2 pixel wider than the target region is a safe bet
	int bw = 2;	// border width
	int xn = max(0, startt.x - bw);
	int yn = max(0, startt.y - bw);
	int heightn = min(_size.height - yn, roiSize.height+bw*2);
	int widthn = min(_size.width - xn, roiSize.width+bw*2);
	IppiPoint startn = {xn, yn};
	IppiSize roiSizen = {widthn, heightn};

	CIPPImage<Ipp8u> maskROI = _mask8u.getRegion(startn, roiSizen);
	// optimize these gradient calls to avoid overhead
	CIPPImage<Ipp8u> imageROIGray = _image8u.getRegion(startn, roiSizen).RGBToGray();
	
	setBoundary(startn, roiSizen);
	setPriorityMap(startn, roiSizen);

	// HOWARD : TODO 
	// optimize these gradient calls to avoid overhead
	// update all the intermediate files
	CIPPImage<Ipp16s> DxROI = getGradientSameSize(imageROIGray, IPPIMAGE_HORIZONTAL);
	CIPPImage<Ipp16s> DyROI = getGradientSameSize(imageROIGray, IPPIMAGE_VERTICAL);
	// ------------------------------------------------------------------------
	// 2. normal of the propagating front is the normalized gradient of the mask
	// ------------------------------------------------------------------------
	CIPPImage<Ipp16s> NxROI = getGradientSameSize(maskROI, IPPIMAGE_HORIZONTAL);
	CIPPImage<Ipp16s> NyROI = getGradientSameSize(maskROI, IPPIMAGE_VERTICAL);
	//CIPPImage<Ipp16s> DxROI(roiSizen, 1, 0);
	//CIPPImage<Ipp16s> DyROI(roiSizen, 1, 0);
	//CIPPImage<Ipp16s> NxROI(roiSizen, 1, 0);
	//CIPPImage<Ipp16s> NyROI(roiSizen, 1, 0);
	//getGradientSimple(imageROIGray, DxROI, DyROI);
	//getGradientSimple(maskROI, NxROI, NyROI);


	// now we crop the ROI region and insert the valid region
	// into the original images
	IppiPoint start0 = {xn+1, yn+1};
	IppiPoint start1 = {1, 1};
	IppiSize size0 = {widthn-2, heightn-2};
	
	_imageGray.copyRegion(start0, imageROIGray, start1, size0);
	_Dx.copyRegion(start0, DxROI, start1, size0);
	_Dy.copyRegion(start0, DyROI, start1, size0);
	_Nx.copyRegion(start0, NxROI, start1, size0);
	_Ny.copyRegion(start0, NyROI, start1, size0);
}



// source map is only set up once.
void CInpainter::initSourceMap(void) 
{
	int height = _size.height;
	int width = _size.width;
	int heightW = _sizeWindow.height;
	int widthW = _sizeWindow.width;
	Ipp8u max;
	long p, offset;

	IppiSize roiSize = {widthW * 2+1, heightW*2+1};

	// check for a window around the pixel, if all of the points within the window are source pixels, then this patch can be used as a source patch
	for(int y = 0; y<height; y++)
		for(int x = 0; x<width; x++)
		{
			p = y*width+x;
			// boundary case: not enough pixel to form a window
			if(x < widthW || y < heightW || x >= width - widthW || y >= height- heightW)
				_sourceMap._imageData[p] = 0;
			else
			{
				offset = (y - heightW) * width + (x-widthW);
				ippiMax_8u_C1R(_mask8u._imageData + offset, width,  roiSize, &max);
				
				// if not all pixels inside the window are source pixels
				if(max != INPAINTER_SOURCE) _sourceMap._imageData[p] = 0;
				else						_sourceMap._imageData[p] = 1;
			}
		}
}



void getGradientSimple(const CIPPImage<Ipp8u> &image, CIPPImage<Ipp16s> &Dx, CIPPImage<Ipp16s> &Dy)
{
	long p;
	int width = image._size.width;
	for(int y = 0; y < image._size.height; y++)
	for(int x = 0; x < width; x++)
	{
		p = y * width + x;
		if(y)Dy._imageData[p] = image._imageData[p] - image._imageData[p-width];
		else Dy._imageData[p] = 0;
		if(x)Dx._imageData[p] = image._imageData[p] - image._imageData[p-1];
		else Dx._imageData[p] = 0;
	} 
}
