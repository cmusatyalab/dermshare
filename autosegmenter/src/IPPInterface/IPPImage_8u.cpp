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

/** =================================================================\n
 *
 * Terrain2\n
 * @file IPPImage_8u.cpp
 *
 * Primary Author: Howard Zhou (howardz@cc.gatech.edu)\n
 *
 *
 * ==================================================================\n
 * Basic image class that serves as an interface
 * for Intel's IPP library\n
 * functions connects ipp with OpenCV's highgui
 * 
 * @version 1.0\n
 *     Initial Revision
 * @date 2004-12-31
 * @author howardz
 *
 *
 * @todo 
 * @bug
 * @warning 
 *
 * ===================================================================\n
 */

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include "IPPImage.h"

/// @namespace using standard namespace
using namespace std;




/**
 * Initialize the image with given size and number of channels
 *
 * @param size IppiSize structure size of the image
 * @param nChannels number of channels
 */
template<>
CIPPImage<Ipp8u>::CIPPImage(IppiSize size, int nChannels )
{
	if(nChannels == 1)
		_imageData = ippiMalloc_8u_C1	( size.width, size.height, &_stepBytes);
	else if(nChannels == 3)
		_imageData = ippiMalloc_8u_C3	( size.width, size.height, &_stepBytes);
	else // if(img->nChannels == 4)
		_imageData = ippiMalloc_8u_AC4   ( size.width, size.height, &_stepBytes);

	_size = size;
	_nChannels = nChannels;
}

template<>
CIPPImage<Ipp8u>::CIPPImage(int width, int height, int nChannels )
{
	if(nChannels == 1)
		_imageData = ippiMalloc_8u_C1	( width, height, &_stepBytes);
	else if(nChannels == 3)
		_imageData = ippiMalloc_8u_C3	( width, height, &_stepBytes);
	else // if(img->nChannels == 4)
		_imageData = ippiMalloc_8u_AC4   ( width, height, &_stepBytes);
	_size.width = width;
	_size.height = height;
	_nChannels = nChannels;
}

template<>
CIPPImage<Ipp8u>::CIPPImage(IppiSize size, int nChannels, const Ipp8u value)
{
	_size = size;
	_nChannels = nChannels;

	if(_nChannels == 1)
	{
		_imageData = ippiMalloc_8u_C1	( _size.width, _size.height, &_stepBytes);
		ippiSet_8u_C1R(value, _imageData, _size.width*_nChannels, _size);
	}
	else if(_nChannels == 3)
	{
		Ipp8u values[3] = {value, value, value};
		_imageData = ippiMalloc_8u_C3	( _size.width, _size.height, &_stepBytes);
		ippiSet_8u_C3R(values, _imageData, _size.width*_nChannels, _size);
	}
	else // if(img->nChannels == 4)
	{
		Ipp8u values[4] = {value, value, value, value};
		_imageData = ippiMalloc_8u_AC4   ( _size.width, _size.height, &_stepBytes);
		ippiSet_8u_AC4R(values, _imageData, _size.width*_nChannels, _size);
	}
}

template<>
CIPPImage<Ipp8u>::CIPPImage(int width, int height, int nChannels, const Ipp8u value )
{
	_size.width = width;
	_size.height = height;
	_nChannels = nChannels;

	if(_nChannels == 1)
	{
		_imageData = ippiMalloc_8u_C1	( _size.width, _size.height, &_stepBytes);
		ippiSet_8u_C1R(value, _imageData, _size.width*_nChannels, _size);
	}
	else if(_nChannels == 3)
	{
		Ipp8u values[3] = {value, value, value};
		_imageData = ippiMalloc_8u_C3	( _size.width, _size.height, &_stepBytes);
		ippiSet_8u_C3R(values, _imageData, _size.width*_nChannels, _size);
	}
	else // if(img->nChannels == 4)
	{
		Ipp8u values[4] = {value, value, value, value};
		_imageData = ippiMalloc_8u_AC4   ( _size.width, _size.height, &_stepBytes);
		ippiSet_8u_AC4R(values, _imageData, _size.width*_nChannels, _size);
	}
}


template<>
CIPPImage<Ipp8u>::CIPPImage(int width, int height, int nChannels, const Ipp8u* data )
{	
	_size.width = width;
	_size.height = height;
	_nChannels = nChannels;

	if(_nChannels == 1)
	{
		// memory allocation for image
		_imageData = ippiMalloc_8u_C1( _size.width, _size.height, &_stepBytes);

		// the pointer of our OpenCV data is our IPP image pointer
		ippiCopy_8u_C1R ( data, width*_nChannels , _imageData, _size.width * _nChannels, _size );

		// function ippiMalloc allocates a memory block aligned to a 32-byte 
		// boundary for elements of different data types.
		// step size should be stepBytes
	}
	else if(_nChannels == 3)
	{
		_imageData = ippiMalloc_8u_C3( _size.width, _size.height, &_stepBytes);
		ippiCopy_8u_C3R ( data, width*_nChannels, _imageData, _size.width * _nChannels , _size );
	}
	else // if(img->_nChannels == 4)
	{
		_imageData = ippiMalloc_8u_AC4( _size.width, _size.height, &_stepBytes);
		ippiCopy_8u_AC4R( data, width*_nChannels, _imageData, _size.width * _nChannels, _size );
	}
}

/**
 * copy constructor
 **/
template<>
CIPPImage<Ipp8u>::CIPPImage(const CIPPImage<Ipp8u> &refImage)
{
	assert(&refImage != NULL);
	if (!refImage.isInitialized()) {
		_imageData = NULL;
		return;
	}

	_nChannels = refImage._nChannels;
	_size = refImage._size;

	if(_nChannels == 1)
	{
		_imageData = ippiMalloc_8u_C1	( _size.width, _size.height, &_stepBytes);
		ippiCopy_8u_C1R ( refImage._imageData, _size.width * _nChannels, _imageData, _size.width * _nChannels, _size );
	}
	else if(_nChannels == 3)
	{
		_imageData = ippiMalloc_8u_C3	( _size.width, _size.height, &_stepBytes);
		ippiCopy_8u_C3R ( refImage._imageData, _size.width * _nChannels, _imageData, _size.width * _nChannels, _size );
	}
	else // if(img->nChannels == 4)
	{
		_imageData = ippiMalloc_8u_AC4   ( _size.width, _size.height, &_stepBytes);
		ippiCopy_8u_AC4R ( refImage._imageData, _size.width * _nChannels, _imageData, _size.width * _nChannels, _size );
	}
}


// ------------------------------------------------------
// Overloaded Operators 
// ------------------------------------------------------

// assignment operator

template<>
CIPPImage<Ipp8u>& CIPPImage<Ipp8u>::operator = (const CIPPImage<Ipp8u> &refImage)
{
	assert(&refImage != NULL);
	// handle self-assignment
	if(this == &refImage) return *this;

	if(NULL != _imageData)
	{
		ippiFree(_imageData);
		_imageData = NULL;
	}

	assert(refImage.isInitialized());

	_nChannels = refImage._nChannels;
	_size = refImage._size;

	if(_nChannels == 1)
	{
		_imageData = ippiMalloc_8u_C1	( _size.width, _size.height, &_stepBytes);
		ippiCopy_8u_C1R ( refImage._imageData, _size.width * _nChannels, _imageData, _size.width * _nChannels, _size );
	}
	else if(_nChannels == 3)
	{
		_imageData = ippiMalloc_8u_C3	( _size.width, _size.height, &_stepBytes);
		ippiCopy_8u_C3R ( refImage._imageData, _size.width * _nChannels, _imageData, _size.width * _nChannels, _size );
	}
	else // if(img->nChannels == 4)
	{
		_imageData = ippiMalloc_8u_AC4   ( _size.width, _size.height, &_stepBytes);
		ippiCopy_8u_AC4R ( refImage._imageData, _size.width * _nChannels, _imageData, _size.width * _nChannels, _size );
	}
	return (*this);
}


template<>	
CIPPImage<Ipp8u> CIPPImage<Ipp8u>::operator+ (const CIPPImage<Ipp8u> &refImage) const
{
	CIPPImage<Ipp8u> result(_size, _nChannels);

	ippiAdd_8u_C1RSfs(refImage._imageData, refImage._size.width, _imageData, _size.width, 
						result._imageData, _size.width, _size, 0);  
	return result;
}

// notice that mask must be an Ipp8u image with value between 0 and 1
template<>
CIPPImage<Ipp8u> CIPPImage<Ipp8u>::operator* (const CIPPImage<Ipp8u> &mask)const
{
	assert(isInitialized() && mask.isInitialized() && _nChannels == mask._nChannels);
	CIPPImage<Ipp8u> result(_size, _nChannels);
	switch(_nChannels)
	{
	case 1:
		ippiMul_8u_C1RSfs(_imageData, _size.width, mask._imageData, mask._size.width, 
					result._imageData, result._size.width, mask._size, 0);
		break;
	case 3:
		ippiMul_8u_C3RSfs(_imageData, _size.width*_nChannels, mask._imageData, mask._size.width*_nChannels, 
					result._imageData, result._size.width*_nChannels, mask._size, 0);
		break;
	case 4:
		ippiMul_8u_AC4RSfs(_imageData, _size.width*_nChannels, mask._imageData, mask._size.width*_nChannels, 
					result._imageData, result._size.width*_nChannels, mask._size, 0);
		break;
	default:
		break;
	}
	return result;
}

template<>
CIPPImage<Ipp8u> CIPPImage<Ipp8u>::operator* (Ipp8u value)const
{
	CIPPImage<Ipp8u> result(_size, _nChannels);
	ippiMulC_8u_C1RSfs(_imageData, _size.width, value, result._imageData, result._size.width, result._size, 0);

	return result;
}


template<>
CIPPImage<Ipp8u> CIPPImage<Ipp8u>::operator| (const CIPPImage<Ipp8u> &refImage) const
{
	assert(_size.width == refImage._size.width &&
		_size.height == refImage._size.height &&
		_nChannels == 1 &&
		refImage._nChannels == 1);

	CIPPImage<Ipp8u> result(_size, _nChannels);
	IppStatus status = ippiOr_8u_C1R(_imageData, _size.width,
		refImage._imageData, refImage._size.width,
		result._imageData, result._size.width, _size);

	if (status)
		displayStatus(status);
	return result;
}


template<>
CIPPImage<Ipp8u> CIPPImage<Ipp8u>::operator& (const CIPPImage<Ipp8u> &refImage) const
{
	assert(_size.width == refImage._size.width &&
		_size.height == refImage._size.height &&
		_nChannels == 1 &&
		refImage._nChannels == 1);

	CIPPImage<Ipp8u> result(_size, _nChannels);
	IppStatus status = ippiAnd_8u_C1R(_imageData, _size.width,
		refImage._imageData, refImage._size.width,
		result._imageData, result._size.width, _size);

	if (status)
		displayStatus(status);
	return result;
}


template<>
CIPPImage<Ipp8u> CIPPImage<Ipp8u>::operator- (void) const
{
	return ((*this)*(-1));
}

// multiply the mask with the image at the center
// TODO: make sure the mask and the original image agrees with size,
// that is, no out of boundary multiplication
template<>
CIPPImage<Ipp8u> CIPPImage<Ipp8u>::multiplyCenter (const CIPPImage<Ipp8u> &mask, IppiPoint center)
{
	// currently assume, the mask located at the center of the image
	// is contained entirely inside of the image

	CIPPImage<Ipp8u> result(_size, _nChannels, 0);
	int xStart = int(center.x - ceil(double(mask._size.width)/2));
	int yStart = int(center.y - ceil(double(mask._size.width)/2));

	if(xStart >= 0 && yStart >= 0 && 
		xStart + mask._size.width <= _size.width && yStart+mask._size.height <=_size.height)
	{
		long offset = xStart + yStart * _size.width;
		ippiMul_8u_C1RSfs(_imageData + offset, _size.width, mask._imageData, mask._size.width,
			result._imageData+offset, result._size.width, mask._size, 0);
	}
	return result;
}

template<>
IppStatus CIPPImage<Ipp8u>::set(const Ipp8u value)
{
	if(NULL == _imageData)
		return ippStsNullPtrErr;

	if(_nChannels == 1)
	{
		return ippiSet_8u_C1R(value, _imageData, _size.width*_nChannels, _size);
	}
	else if(_nChannels == 3)
	{
		Ipp8u values[3] = {value, value, value};
		return ippiSet_8u_C3R(values, _imageData, _size.width*_nChannels, _size);
	}
	else // if(img->nChannels == 4)
	{
		Ipp8u values[4] = {value, value, value, value};
		return ippiSet_8u_AC4R(values, _imageData, _size.width*_nChannels, _size);
	}
}

template<>
IppStatus CIPPImage<Ipp8u>::set(const Ipp8u value, const CIPPImage<Ipp8u> &mask)
{
	if(NULL == _imageData)
		return ippStsNullPtrErr;

	assert(mask._size.height == _size.height && mask._size.width == _size.width);

	if(_nChannels != 1)
		return ippStsNullPtrErr;	
		
	return ippiSet_8u_C1MR(value, _imageData, _size.width, _size, mask._imageData, _size.width);
}


template<>
IppStatus CIPPImage<Ipp8u>::setRegion(const Ipp8u value, IppiPoint start, IppiSize sizeROI)
{
	if(NULL == _imageData)
		return ippStsNullPtrErr;

	long offset = (start.x + start.y * _size.width) *_nChannels;

	if(_nChannels == 1)
	{	return ippiSet_8u_C1R(value, _imageData + offset, _size.width*_nChannels, sizeROI);
	}
	else if(_nChannels == 3)
	{	Ipp8u values[3] = {value, value, value};
		return ippiSet_8u_C3R(values, _imageData + offset, _size.width*_nChannels, sizeROI);
	}
	else // if(img->nChannels == 4)
	{	Ipp8u values[4] = {value, value, value, value};
		return ippiSet_8u_AC4R(values, _imageData + offset, _size.width*_nChannels, sizeROI);
	}
}


template<>
CIPPImage<Ipp8u> CIPPImage<Ipp8u>::getRegion(IppiPoint start, IppiSize sizeROI) const
{

	CIPPImage<Ipp8u> result(sizeROI, _nChannels, 0);
	long offset = (start.x + start.y * _size.width) *_nChannels;

	if(_nChannels == 1)
	{	ippiCopy_8u_C1R(_imageData + offset, _size.width*_nChannels, 
					result._imageData, result._size.width*_nChannels, sizeROI);
	}
	else if(_nChannels == 3)
	{	
		ippiCopy_8u_C3R(_imageData + offset, _size.width*_nChannels, 
					result._imageData, result._size.width*_nChannels, sizeROI);
	}
	else // if(img->nChannels == 4)
	{	
		ippiCopy_8u_AC4R(_imageData + offset, _size.width*_nChannels, 
					result._imageData, result._size.width*_nChannels, sizeROI);
	}
	return result;
}


template<>
IppStatus CIPPImage<Ipp8u>::copyRegion(IppiPoint start, const CIPPImage<Ipp8u> &patch, IppiPoint patchStart, IppiSize sizeROI)
{
	if(NULL == _imageData)
		return ippStsNullPtrErr;

	long offset = (start.x + start.y * _size.width)*_nChannels;
	long patchOffset = (patchStart.x + patchStart.y * patch._size.width)*_nChannels;

	if(_nChannels == 1)
	{	return ippiCopy_8u_C1R(patch._imageData+patchOffset, patch._size.width*_nChannels, _imageData + offset, _size.width*_nChannels, sizeROI);
	}
	else if(_nChannels == 3)
	{	
		return ippiCopy_8u_C3R(patch._imageData+patchOffset, patch._size.width*_nChannels, _imageData + offset, _size.width*_nChannels, sizeROI);
	}
	else // if(img->nChannels == 4)
	{	
		return ippiCopy_8u_AC4R(patch._imageData+patchOffset, patch._size.width*_nChannels, _imageData + offset, _size.width*_nChannels, sizeROI);
	}
}

// mask must be of the same size as the patch
template<>
IppStatus CIPPImage<Ipp8u>::copyRegionMask(IppiPoint start, const CIPPImage<Ipp8u> &patch, IppiPoint patchStart, 
										   IppiSize sizeROI, const CIPPImage<Ipp8u> &mask)
{
	assert(mask._nChannels == 1);
	if(NULL == _imageData)
		return ippStsNullPtrErr;

	long offset = (start.x + start.y * _size.width)*_nChannels;
	long patchOffset = (patchStart.x + patchStart.y * patch._size.width)*_nChannels;

	// HOWARD : 2007-06-10
	if(_nChannels == 1)
	{	
		return ippiCopy_8u_C1MR(patch._imageData+patchOffset, patch._size.width*_nChannels, 
			_imageData + offset, _size.width*_nChannels, sizeROI, mask._imageData, mask._size.width);
	}
	else if(_nChannels == 3)
	{	
		return ippiCopy_8u_C3MR(patch._imageData+patchOffset, patch._size.width*_nChannels, 
			_imageData + offset, _size.width*_nChannels, sizeROI, mask._imageData, mask._size.width);
	}
	else // if(img->nChannels == 4)
	{	
		return ippiCopy_8u_AC4MR(patch._imageData+patchOffset, patch._size.width*_nChannels, 
			_imageData + offset, _size.width*_nChannels, sizeROI, mask._imageData, mask._size.width);
	}
}

// mask must be of the same size as the patch
template<>
IppStatus CIPPImage<Ipp8u>::copyRegionMasked(IppiPoint start, const CIPPImage<Ipp8u> &patch, IppiPoint patchStart, 
										   IppiSize sizeROI, const CIPPImage<Ipp8u> &mask, IppiPoint maskStart)
{
	assert(mask._nChannels == 1);
	if(NULL == _imageData)
		return ippStsNullPtrErr;

	long offset = (start.x + start.y * _size.width)*_nChannels;
	long patchOffset = (patchStart.x + patchStart.y * patch._size.width)*_nChannels;
	long maskOffset = (maskStart.x + maskStart.y * mask._size.width);

	// HOWARD : 2007-06-10
	if(_nChannels == 1)
	{	
		return ippiCopy_8u_C1MR(patch._imageData+patchOffset, patch._size.width*_nChannels, 
			_imageData + offset, _size.width*_nChannels, sizeROI, mask._imageData+maskOffset, mask._size.width);
	}
	else if(_nChannels == 3)
	{	
		return ippiCopy_8u_C3MR(patch._imageData+patchOffset, patch._size.width*_nChannels, 
			_imageData + offset, _size.width*_nChannels, sizeROI, mask._imageData+maskOffset, mask._size.width);
	}
	else // if(img->nChannels == 4)
	{	
		return ippiCopy_8u_AC4MR(patch._imageData+patchOffset, patch._size.width*_nChannels, 
			_imageData + offset, _size.width*_nChannels, sizeROI, mask._imageData+maskOffset, mask._size.width);
	}
}

template<>
IppStatus CIPPImage<Ipp8u>::copyReplicateBorder(IppiPoint start, IppiSize sizeROI, 
												const CIPPImage<Ipp8u> &src, IppiPoint srcStart, IppiSize sizeSrcROI, 
												int topBorderWidth, int leftBorderWidth)
{
	if(NULL == _imageData)
		return ippStsNullPtrErr;

	long offset = start.x + start.y * _size.width;
	long srcOffset = srcStart.x + srcStart.y * src._size.width;

	if(_nChannels == 1)
	{	return ippiCopyReplicateBorder_8u_C1R(
			src._imageData+srcOffset, src._size.width * _nChannels, sizeSrcROI, 
			_imageData + offset, _size.width * _nChannels, sizeROI,
			topBorderWidth, leftBorderWidth);
	}
	else if(_nChannels == 3)
	{	
		return ippiCopyReplicateBorder_8u_C3R(
			src._imageData+srcOffset, src._size.width * _nChannels, sizeSrcROI, 
			_imageData + offset, _size.width * _nChannels, sizeROI,
			topBorderWidth, leftBorderWidth);
	}
	else // if(img->nChannels == 4)
	{	
		return ippiCopyReplicateBorder_8u_AC4R(
			src._imageData+srcOffset, src._size.width * _nChannels, sizeSrcROI, 
			_imageData + offset, _size.width * _nChannels, sizeROI,
			topBorderWidth, leftBorderWidth);
	}
}

template<>
double CIPPImage<Ipp8u>::sum() const
{
	Ipp64f result;
	ippiSum_8u_C1R(_imageData, _size.width, _size, &result);
	return double(result);
}

template<>
double CIPPImage<Ipp8u>::sum(IppiPoint start, IppiSize sizeROI) const
{
	Ipp64f result;
	int offset = start.x + start.y * _size.width;
	ippiSum_8u_C1R(_imageData + offset, _size.width, sizeROI, &result);
	return double(result);
}


template<>
Ipp8u CIPPImage<Ipp8u>::getMax(int *pIndexX, int *pIndexY) const
{
	Ipp8u value;

	ippiMaxIndx_8u_C1R(_imageData, _size.width, _size, &value, pIndexX, pIndexY); 
	return value;
}
	
template<>
Ipp8u CIPPImage<Ipp8u>::getMin(int *pIndexX, int *pIndexY) const
{
	Ipp8u value;

	ippiMinIndx_8u_C1R(_imageData, _size.width, _size, &value, pIndexX, pIndexY); 
	return value;
}

/**
 * IppStatus load( char *filename )
 * loadImage used to display IPP images with OpenCV highgui
 * @param name the name of the image file to load
 * @param iscolor (default = -1 in declaration)
 *        Specifies colorness of the loaded image:\n
 *        if >0, the loaded image is forced to be color 3-channel image;\n
 *        if 0, the loaded image is forced to be grayscale;\n
 *        if <0, the loaded image will be loaded as is (with number of channels depends on the file). 
 * @return bool is the memory allocation successful
 *
 *
 * The function cvLoadImage loads an image from the specified file and returns the pointer to the 
 * loaded image. Currently the following file formats are supported: 
 * 
 * Windows bitmaps - BMP, DIB; 
 * JPEG files - JPEG, JPG, JPE; 
 * Portable Network Graphics - PNG; 
 * Portable image format - PBM, PGM, PPM; 
 * Sun rasters - SR, RAS; 
 * TIFF files - TIFF, TIF. 
 */
template<>
IppStatus CIPPImage<Ipp8u>::load(const char *filename, int fileType, int isColor )
{	
	if(NULL != _imageData)
	{
		ippiFree(_imageData);
		_imageData = NULL;
	}

	IppStatus status;

	IplImage* img = NULL; // new IplImage structure img

	// load an image file into img
	if (NULL == (img = cvLoadImage( filename, isColor )))
	{
		printf("\nError -- file %s doesn't exist", filename);
		return ippStsNotSupportedModeErr;
	}

	_size.width = img->width; // pSize of IPP images is the same
	_size.height = img->height; // as read image img
	_nChannels = img->nChannels;

	if(img->depth != IPL_DEPTH_8U)
	{
		printf("\nError -- image depth is not 8u");
		return ippStsNotSupportedModeErr;
	}

	if(_nChannels == 1)
	{
		// memory allocation for image
		_imageData = ippiMalloc_8u_C1( _size.width, _size.height, &_stepBytes);

		// the pointer of our OpenCV data is our IPP image pointer
		status = ippiCopy_8u_C1R ( (Ipp8u*)img->imageData, img->widthStep, _imageData, _size.width * _nChannels, _size );

		// function ippiMalloc allocates a memory block aligned to a 32-byte 
		// boundary for elements of different data types.
		// step size should be stepBytes
	}
	else if(_nChannels == 3)
	{
		//cout << "\nColored 24-bit image\n";
		_imageData = ippiMalloc_8u_C3( _size.width, _size.height, &_stepBytes);
		status = ippiCopy_8u_C3R ( (Ipp8u*)img->imageData, img->widthStep, _imageData, _size.width * _nChannels , _size );
	}
	else // if(img->_nChannels == 4)
	{
		_imageData = ippiMalloc_8u_AC4( _size.width, _size.height, &_stepBytes);
		status = ippiCopy_8u_AC4R( (Ipp8u*)img->imageData, img->widthStep, _imageData, _size.width * _nChannels, _size );
	}

	// memory release with OpenCV
	cvReleaseImage(&img);
	return status;
}

template<>
IppStatus CIPPImage<Ipp8u>::load(const void *data, size_t len, int isColor)
{
	if(NULL != _imageData)
	{
		ippiFree(_imageData);
		_imageData = NULL;
	}

	IppStatus status;

	CvMat *mat = cvCreateMatHeader(1, len, CV_8UC1);
	cvSetData(mat, (void *) data, len);
	IplImage* img = cvDecodeImage(mat, isColor);
	cvReleaseMat(&mat);

	if (img == NULL) {
		printf("Error -- couldn't decode image\n");
		return ippStsNotSupportedModeErr;
	}

	_size.width = img->width; // pSize of IPP images is the same
	_size.height = img->height; // as read image img
	_nChannels = img->nChannels;

	if(img->depth != IPL_DEPTH_8U)
	{
		printf("\nError -- image depth is not 8u");
		return ippStsNotSupportedModeErr;
	}

	if(_nChannels == 1)
	{
		// memory allocation for image
		_imageData = ippiMalloc_8u_C1( _size.width, _size.height, &_stepBytes);

		// the pointer of our OpenCV data is our IPP image pointer
		status = ippiCopy_8u_C1R ( (Ipp8u*)img->imageData, img->widthStep, _imageData, _size.width * _nChannels, _size );

		// function ippiMalloc allocates a memory block aligned to a 32-byte
		// boundary for elements of different data types.
		// step size should be stepBytes
	}
	else if(_nChannels == 3)
	{
		//cout << "\nColored 24-bit image\n";
		_imageData = ippiMalloc_8u_C3( _size.width, _size.height, &_stepBytes);
		status = ippiCopy_8u_C3R ( (Ipp8u*)img->imageData, img->widthStep, _imageData, _size.width * _nChannels , _size );
	}
	else // if(img->_nChannels == 4)
	{
		_imageData = ippiMalloc_8u_AC4( _size.width, _size.height, &_stepBytes);
		status = ippiCopy_8u_AC4R( (Ipp8u*)img->imageData, img->widthStep, _imageData, _size.width * _nChannels, _size );
	}

	// memory release with OpenCV
	cvReleaseImage(&img);
	return status;
}

/**
 * IppStatus load(const RGBImage &image)
 * load a 3-channel color image from a Diamond RGBImage
 */
template<>
IppStatus CIPPImage<Ipp8u>::load(const RGBImage &image)
{
	if(NULL != _imageData)
	{
		ippiFree(_imageData);
		_imageData = NULL;
	}

	_size.width = image.width;
	_size.height = image.height;
	_nChannels = 3;

	_imageData = ippiMalloc_8u_C3(_size.width, _size.height, &_stepBytes);
	const int channels[] = {2, 1, 0};
	return ippiSwapChannels_8u_C4C3R((const Ipp8u *) image.data, image.width * 4, _imageData, image.width * 3, _size, channels);
}


/**
 * IppStatus CIPPImage_8u::save(char *filename )
 * saveImage saves the given image into as the file with the given filename with OpenCV highgui
 * @param filename the name of the file
 * @return IppStatus
 * 
 * @note The function cvSaveImage saves the image to the specified file. 
 * Only 8-bit single-channel or 3-channel (with &apos;BGR&apos; channel order) images can be saved 
 * using this function. If the format, depth or channel order is different, 
 * use cvCvtScale and cvCvtColor to convert it before saving, or use universal 
 * cvSave to save the image to XML or YAML format. 
 **/
template<>
IppStatus CIPPImage<Ipp8u>::save(const char *filename, int fileType) const
{
	IplImage *cvImg; // cv image used to save
	CvSize sizeCv; // cv size structure
	Ipp8u *tmp; // IPP temporary image

	IppStatus status;

	sizeCv.width = _size.width; // define size
	sizeCv.height = _size.height; // of th displayed image
	
	cvImg = cvCreateImageHeader( sizeCv, IPL_DEPTH_8U, _nChannels );

	int stepBytes;
	if(_nChannels == 1)
	{
		tmp = ippiMalloc_8u_C1( _size.width, _size.height, &stepBytes );
		status = ippiCopy_8u_C1R ( _imageData, _size.width * _nChannels, tmp, _size.width * _nChannels, _size );
	}
	else if (_nChannels == 3)
	{
		tmp = ippiMalloc_8u_C3( _size.width, _size.height, &stepBytes );
		status = ippiCopy_8u_C3R ( _imageData, _size.width * _nChannels, tmp, _size.width * _nChannels, _size );
	}
	else if (_nChannels == 4)
	{
		tmp = ippiMalloc_8u_AC4( _size.width, _size.height, &stepBytes );
		status = ippiCopy_8u_AC4R( _imageData, _size.width * _nChannels, tmp, _size.width * _nChannels, _size );
	}
	else
	{
		printf("Error -- _nChannels can only be 1, 3, or 4");
		return ippStsNotSupportedModeErr;
	}

	// set cv image to contain tmp
	cvSetData( cvImg, (void *) tmp, sizeCv.width * _nChannels );
	cvSaveImage( filename, cvImg ); // save image to disk

	// free
	cvReleaseImageHeader( &cvImg );
	ippiFree(tmp);
	tmp = NULL;

	return status;
}

/**
 * IppStatus CIPPImage_8u::save(const char *extension, vector<uchar> &buf)
 **/
template<>
IppStatus CIPPImage<Ipp8u>::save(const char *extension, vector<uchar> &buf) const
{
	CvSize sizeCv; // cv size structure
	Ipp8u *tmp; // IPP temporary image

	IppStatus status;

	sizeCv.width = _size.width;
	sizeCv.height = _size.height;

	IplImage *cvImg = cvCreateImageHeader( sizeCv, IPL_DEPTH_8U, _nChannels );

	int stepBytes;
	if(_nChannels == 1)
	{
		tmp = ippiMalloc_8u_C1( _size.width, _size.height, &stepBytes );
		status = ippiCopy_8u_C1R ( _imageData, _size.width * _nChannels, tmp, _size.width * _nChannels, _size );
	}
	else if (_nChannels == 3)
	{
		tmp = ippiMalloc_8u_C3( _size.width, _size.height, &stepBytes );
		status = ippiCopy_8u_C3R ( _imageData, _size.width * _nChannels, tmp, _size.width * _nChannels, _size );
	}
	else if (_nChannels == 4)
	{
		tmp = ippiMalloc_8u_AC4( _size.width, _size.height, &stepBytes );
		status = ippiCopy_8u_AC4R( _imageData, _size.width * _nChannels, tmp, _size.width * _nChannels, _size );
	}
	else
	{
		printf("Error -- _nChannels can only be 1, 3, or 4");
		return ippStsNotSupportedModeErr;
	}

	// set cv image to contain tmp
	cvSetData( cvImg, (void *) tmp, sizeCv.width * _nChannels );

	CvMat *mat = cvEncodeImage(extension, cvImg, NULL);
	buf.assign(mat->data.ptr, mat->data.ptr + mat->cols);
	cvReleaseMat(&mat);

	// free
	cvReleaseImageHeader( &cvImg );
	ippiFree(tmp);
	tmp = NULL;

	return status;
}

/**
 * IppStatus CIPPImage_8u::saveScaled(char *filename )
 * saveImage saves the given image into as the file with the given filename with OpenCV highgui
 * @param filename the name of the file
 * @return IppStatus
 **/
template<>
IppStatus CIPPImage<Ipp8u>::saveScaled(const char *filename, int fileType) const
{
	IplImage *cvImg; // cv image used to save
	CvSize sizeCv; // cv size structure
	Ipp8u *tmp = NULL; // IPP temporary image

	IppStatus status = ippStsNotSupportedModeErr;

	sizeCv.width = _size.width; // define size
	sizeCv.height = _size.height; // of th displayed image
	
	cvImg = cvCreateImageHeader( sizeCv, IPL_DEPTH_8U, _nChannels );

	int stepBytes;
	Ipp8u max;

	if(_nChannels == 1)
	{
		Ipp32f *tmpScale = ippiMalloc_32f_C1( _size.width, _size.height, &stepBytes);

		// get the max and min of the image
		// rescale the image so we can display it
		// ippiMin_8u_C1R(image, _size.width, _size, &min);
		ippiMax_8u_C1R(_imageData, _size.width, _size, &max);

		ippiConvert_8u32f_C1R(_imageData, _size.width, tmpScale, _size.width*4, _size);

		// for each pixel p, value = (ov - 0) * 255/max
		Ipp32f ratio;
		if (0 == max)
			ratio = 0;
		else
			ratio = 255.0f/max;
		ippiMulC_32f_C1IR(ratio, tmpScale, _size.width*4, _size); 

		tmp = ippiMalloc_8u_C1( _size.width, _size.height, &stepBytes);
		status = ippiScale_32f8u_C1R(tmpScale, _size.width * 4, tmp, _size.width, _size, 0, 255);

		ippiFree(tmpScale);
		tmpScale = NULL;
	}
	else if (_nChannels == 3)
	{
		tmp = ippiMalloc_8u_C3( _size.width, _size.height, &stepBytes);
		status = ippiCopy_8u_C3R ( _imageData, _size.width * _nChannels, tmp, _size.width * _nChannels, _size );
	}
	else if (_nChannels == 4)
	{
		tmp = ippiMalloc_8u_AC4( _size.width, _size.height, &stepBytes);
		status = ippiCopy_8u_AC4R( _imageData, _size.width * _nChannels, tmp, _size.width * _nChannels, _size );
	}
	else
	{
		printf("Error -- _nChannels can only be 1, 3, or 4");
		return ippStsNotSupportedModeErr;
	}
	
	// set cv image to contain tmp
	cvSetData( cvImg, (void *) tmp, sizeCv.width * _nChannels );
	cvSaveImage( filename, cvImg ); // save image to disk

	// free
	cvReleaseImageHeader( &cvImg );
	ippiFree(tmp);
	tmp = NULL;

	return status;
}

/**
 * IppStatus getGradient(const Ipp8u* pSrc, IppiSize sizeSrc, Ipp16s* &pDst, IppiSize * pSizeDst, int nChannels, int direction)
 * get the gradient of the source image
 * @param pSrc pointer to the source image
 * @param sizeSrc size of the source image
 * @param pDst reference pointer to the destination image
 * @param pSizeDst pointer to the size of the destination image
 * @param nChannels number of channels
 * @param direction horizontal or vertical direction)
 * @return IppStatus status of the filtering operation
 **/
CIPPImage<Ipp16s> getGradient(const CIPPImage<Ipp8u> &image, int direction)
{
	CIPPImage<Ipp16s> temp = convert_8u16s(image);
	return temp.getGradient(direction);	
}

CIPPImage<Ipp16s> getGradientSameSize(const CIPPImage<Ipp8u> &image, int direction)
{
	CIPPImage<Ipp16s> temp = convert_8u16s(image);
	return temp.getGradientSameSize(direction);
}

CIPPImage<Ipp16s> getGradientMagnitude(const CIPPImage<Ipp8u> &image)
{
	CIPPImage<Ipp16s> temp = convert_8u16s(image);
	return temp.getGradientMagnitude();
}

/**
 * IppStatus getDivergence(const Ipp8u* pSrc, IppiSize sizeSrc, Ipp16s* &pDst, IppiSize *pSizeDst, int nChannels)
 * get the divergence of the source image, the divergence of the image is the sum of the d^2(x) + d^2(y)
 *
 * @param pSrc pointer to the source image
 * @param sizeSrc size of the source image
 * @param pDst reference pointer to the destination image
 * @param pSizeDst pointer to the size of the destination image
 * @param nChannels number of channels
 * @return IppStatus
 **/
CIPPImage<Ipp16s> getDivergence(const CIPPImage<Ipp8u> &image)
{
	//CIPPImage<Ipp16s> Dx;
	//CIPPImage<Ipp16s> Dy;
	//CIPPImage<Ipp16s> DDx;
	//CIPPImage<Ipp16s> DDy;

	// use forward differencing to filter the image
	// to get the gradient field dx and dy
	//Dx = getGradient(image, IPPIMAGE_HORIZONTAL);
	//Dy = getGradient(image, IPPIMAGE_VERTICAL);
	//CIPPImage<Ipp16s> result = getDivergence(Dx, Dy);
	//return CIPPImage<Ipp16s>(result);

	return getDivergence(getGradient(image, IPPIMAGE_HORIZONTAL), 
		getGradient(image, IPPIMAGE_VERTICAL));
}


CIPPImage<Ipp16s> getDivergence(const CIPPImage<Ipp16s> & Dx, const CIPPImage<Ipp16s> & Dy)
{
	CIPPImage<Ipp16s> DDx;
	CIPPImage<Ipp16s> DDy;

	// get the divergent field using backward differencing
	// and add divx and divy
	DDx = Dx.getGradient(IPPIMAGE_HORIZONTAL);
	DDy = Dy.getGradient(IPPIMAGE_VERTICAL);

	IppiSize sizeResult = {DDx._size.width, DDy._size.height};
	CIPPImage<Ipp16s> result(sizeResult, 1);

	// get the divergent field
	ippiAdd_16s_C1RSfs(DDx._imageData + DDx._size.width , DDx._size.width * 2, 
						DDy._imageData + 1, DDy._size.width * 2, 
						result._imageData, result._size.width * 2, result._size, 0);

	return result;

}

template<>
CIPPImage<Ipp8u> CIPPImage<Ipp8u>::filterGauss(IppiMaskSize mask) const
{
	CIPPImage<Ipp8u> result(_size, _nChannels, 0);
	IppiSize sizeSrcBorder;
	IppiPoint startSrcBorder = {0, 0};
	IppiPoint startSrc = {0, 0};
	CIPPImage<Ipp8u> sourceBorder;
	int ROIOffset;
	if(ippMskSize5x5 == mask)
	{
		sizeSrcBorder.height = _size.height + 4;
		sizeSrcBorder.width = _size.width + 4;
		sourceBorder = CIPPImage<Ipp8u>(sizeSrcBorder, _nChannels, Ipp8u(0));
		sourceBorder.copyReplicateBorder(startSrcBorder, sizeSrcBorder, *this, startSrc, _size, 2, 2);
		ROIOffset = (sizeSrcBorder.width * 2 + 2)*_nChannels;
	}
	else // ippMskSize3x3
	{
		sizeSrcBorder.height = _size.height + 2;
		sizeSrcBorder.width = _size.width + 2;
		sourceBorder = CIPPImage<Ipp8u>(sizeSrcBorder, _nChannels, Ipp8u(0));
		sourceBorder.copyReplicateBorder(startSrcBorder, sizeSrcBorder, *this, startSrc, _size, 1, 1);
		ROIOffset = (sizeSrcBorder.width + 1)*_nChannels;
	}

	switch(_nChannels)
	{
	case 1:
		ippiFilterGauss_8u_C1R(sourceBorder._imageData + ROIOffset, sizeSrcBorder.width*_nChannels, result._imageData, _size.width*_nChannels, _size, mask);
		break;
	case 3:
		ippiFilterGauss_8u_C3R(sourceBorder._imageData + ROIOffset, sizeSrcBorder.width*_nChannels, result._imageData, _size.width*_nChannels, _size, mask);
		break;
	case 4:
		ippiFilterGauss_8u_AC4R(sourceBorder._imageData + ROIOffset, sizeSrcBorder.width*_nChannels, result._imageData, _size.width*_nChannels, _size, mask);
		break;
	}		

	return result;
}

template<>
CIPPImage<Ipp8u> CIPPImage<Ipp8u>::filterLaplace(IppiMaskSize mask) const
{
	CIPPImage<Ipp8u> result(_size, _nChannels, 0);
	IppiSize sizeSrcBorder;
	IppiPoint startSrcBorder = {0, 0};
	IppiPoint startSrc = {0, 0};

	if(ippMskSize5x5 == mask)
	{
		sizeSrcBorder.height = _size.height + 4;
		sizeSrcBorder.width = _size.width + 4;
		CIPPImage<Ipp8u> sourceBorder(sizeSrcBorder, _nChannels, 0);
		sourceBorder.copyReplicateBorder(startSrcBorder, sizeSrcBorder, *this, startSrc, _size, 2, 2);

		int ROIOffset = sizeSrcBorder.width * 2 + 2;
		ippiFilterLaplace_8u_C1R(sourceBorder._imageData + ROIOffset, sizeSrcBorder.width, result._imageData, _size.width, _size, mask);
	}
	else // ippMskSize3x3
	{
		sizeSrcBorder.height = _size.height + 2;
		sizeSrcBorder.width = _size.width + 2;
		CIPPImage<Ipp8u> sourceBorder(sizeSrcBorder, _nChannels, 0);
		sourceBorder.copyReplicateBorder(startSrcBorder, sizeSrcBorder, *this, startSrc, _size, 1, 1);

		int ROIOffset = sizeSrcBorder.width + 1;
		ippiFilterLaplace_8u_C1R(sourceBorder._imageData + ROIOffset, sizeSrcBorder.width, result._imageData, _size.width, _size, mask);
	}
		
	return result;
}


//The median filter functions perform non-linear filtering of a source image data.
//These functions use either an arbitrary rectangular mask, or the following predefined
//masks of the IppiMaskSize type to filter an image:
//ippMskSize3x1 Horizontal mask of length 3
//ippMskSize5x1 Horizontal mask of length 5
//ippMskSize1x3 Vertical mask of length 3
//ippMskSize3x3 Square mask of size 3
//ippMskSize1x5 Vertical mask of length 5
//ippMskSize5x5 Square mask of size 5
//The size of the neighborhood and coordinates of the anchor cell in the neighborhood
//depend on the mask mean. Table 9-2 lists the mask types with the corresponding
//neighborhood sizes and anchor cell coordinates. Note that in mask names the mask size is
//indicated in (XY) order.The anchor cell is specified by its coordinates anchor.x and
//anchor.y in the coordinate system associated with the top left corner of the mask
template<>
CIPPImage<Ipp8u> CIPPImage<Ipp8u>::filterMedian(IppiMaskSize mask) const
{
	CIPPImage<Ipp8u> result(_size, _nChannels, 0);
	IppiSize sizeSrcBorder;
	IppiPoint startSrcBorder = {0, 0};
	IppiPoint startSrc = {0, 0};
	IppiPoint anchor = {0, 0};
	IppiSize maskSize;
	IppStatus status;
	int ROIOffset;
	CIPPImage<Ipp8u> sourceBorder;

	switch(mask)
	{
	case ippMskSize5x5:
		sizeSrcBorder.height = _size.height + 4;
		sizeSrcBorder.width = _size.width + 4;
		sourceBorder = CIPPImage<Ipp8u>(sizeSrcBorder, _nChannels, 0);
		sourceBorder.copyReplicateBorder(startSrcBorder, sizeSrcBorder, *this, startSrc, _size, 2, 2);
		ROIOffset = (sizeSrcBorder.width * 2 + 2)*_nChannels;
		anchor.x = 2;anchor.y = 2;
		maskSize.width = 5;maskSize.height = 5;
		break;
	case ippMskSize3x3:
	default:
		sizeSrcBorder.height = _size.height + 2;
		sizeSrcBorder.width = _size.width + 2;
		sourceBorder = CIPPImage<Ipp8u>(sizeSrcBorder, _nChannels, 0);
		sourceBorder.copyReplicateBorder(startSrcBorder, sizeSrcBorder, *this, startSrc, _size, 1, 1);
		ROIOffset = (sizeSrcBorder.width + 1)*_nChannels;
		anchor.x = 1;anchor.y = 1;
		maskSize.width = 3;maskSize.height = 3;
		break;
	}

	switch(_nChannels)
	{
	case 1:
		status = ippiFilterMedian_8u_C1R(sourceBorder._imageData + ROIOffset, sizeSrcBorder.width*_nChannels, result._imageData, _size.width*_nChannels, _size, maskSize, anchor);
		break;
	case 3:
		status = ippiFilterMedian_8u_C3R(sourceBorder._imageData + ROIOffset, sizeSrcBorder.width*_nChannels, result._imageData, _size.width*_nChannels, _size, maskSize, anchor);
		break;
	case 4:
		status = ippiFilterMedian_8u_AC4R(sourceBorder._imageData + ROIOffset, sizeSrcBorder.width*_nChannels, result._imageData, _size.width*_nChannels, _size, maskSize, anchor);
		break;
	default:
		cout << "\nError -- [CIPPImage<Ipp8u>::filterMedian] invalid number of channels.";
	}

	if (status)
		displayStatus(status);
	return result;
}


/**
 * bool rotateImage(const Ipp8u * pSrc, IppiSize sizeSrc, Ipp8u * &pDst, IppiSize * pSizeDst, int nChannels, double angle, double * pXShift, double * pYShift)\
 * following matlab's style
 *
 * @param angle rotation angle in degree
 * @param pXShift pointer to horizontal shift of the original origin
 * @param pYShift pointer to vertical shift of the original origin
 * @param interpolation type of interpolation used, selected from {IPPI_INTER_NN, IPPI_INTER_LINEAR, IPPI_INTER_CUBIC, IPPI_SMOOTH_EDGE}
 * @return IppStatus
 **/
template<>
CIPPImage<Ipp8u> CIPPImage<Ipp8u>::rotateAroundCenter(double angle, double * pXShift, double * pYShift, int interpolation) const
{
	IppiSize sizeResult;
	IppiRect rectS, rectR;
	IppStatus status;

	double xCenter, yCenter, xShift, yShift;
	double bound[2][2];
	xCenter = ceil(double(_size.width)/2);
	yCenter = ceil(double(_size.height)/2);
	
	rectS.x = 0;
	rectS.y = 0;
	rectS.width = _size.width;
	rectS.height = _size.height;

	ippiGetRotateShift(xCenter, yCenter, angle, &xShift, &yShift);
	ippiGetRotateBound(rectS, bound, angle, xShift, yShift);

	rectR.x = 0;
	rectR.y = 0;
	rectR.width = ROUND(bound[1][0]-bound[0][0]);
	// printf("\n%f, %f, %d", bound[1][0], bound[0][0], rectR.width);

	rectR.height = ROUND(bound[1][1]-bound[0][1]);
	// printf("\n%f, %f, %d", bound[1][1], bound[0][1], rectR.height);

	// TODO:
	// it is impossible for a rotated version of image to be smaller than the original one
	sizeResult = _size;
	if(rectR.width > _size.width)
		sizeResult.width = rectR.width;
	if(rectR.height > _size.height)
		sizeResult.height = rectR.height;

	CIPPImage<Ipp8u> result(sizeResult, _nChannels, 0);

	switch(_nChannels)
	{
		// if only one channel
	case 1:
		// ippiSet_8u_C1R(0, result._imageData, sizeResult.width, sizeResult); 

		status = ippiRotate_8u_C1R(_imageData, _size, _size.width, rectS, 
			result._imageData, sizeResult.width, rectR, angle, 
			xShift - bound[0][0], yShift - bound[0][1], interpolation);
		// printf( " %d : %s\n", status, ippCoreGetStatusString( status));
		break;
	}

	if (status)
		displayStatus(status);
	return result;
}


template<>
CIPPImage<Ipp8u> CIPPImage<Ipp8u>::rotateCenter(double angle, double xCenter, double yCenter, int interpolation) const
{
	IppiSize sizeResult;
	IppiRect rectS, rectR;
	IppStatus status;

	double xShift, yShift;
	double bound[2][2];
	
	rectS.x = 0;
	rectS.y = 0;
	rectS.width = _size.width;
	rectS.height = _size.height;

	ippiGetRotateShift(xCenter, yCenter, angle, &xShift, &yShift);
	ippiGetRotateBound(rectS, bound, angle, xShift, yShift);

	rectR.x = 0;
	rectR.y = 0;
	rectR.width = ROUND(bound[1][0]-bound[0][0]);
	// printf("\n%f, %f, %d", bound[1][0], bound[0][0], rectR.width);

	rectR.height = ROUND(bound[1][1]-bound[0][1]);
	// printf("\n%f, %f, %d", bound[1][1], bound[0][1], rectR.height);

	// TODO:
	// it is impossible for a rotated version of image to be smaller than the original one
	sizeResult = _size;
	if(rectR.width > _size.width)
		sizeResult.width = rectR.width;
	if(rectR.height > _size.height)
		sizeResult.height = rectR.height;

	CIPPImage<Ipp8u> result(sizeResult, _nChannels, 0);

	switch(_nChannels)
	{
		// if only one channel
	case 1:
		// ippiSet_8u_C1R(0, result._imageData, sizeResult.width, sizeResult); 

		status = ippiRotate_8u_C1R(_imageData, _size, _size.width, rectS, 
			result._imageData, sizeResult.width, rectR, angle, 
			xShift - bound[0][0], yShift - bound[0][1], interpolation);
		// printf( " %d : %s\n", status, ippCoreGetStatusString( status));
		break;
	}

	if (status)
		displayStatus(status);
	return result;
}



template<>
CIPPImage<Ipp8u> CIPPImage<Ipp8u>::getRotatedSquarePatch(IppiPoint rotationAxis, double angleInDegree, int radius) const
{

	// the large radius needed for the rotation
	//int radiusLarge = (int)ceil(sqrt(double(2.0))*radius);
	int radiusLarge = int(ceil(radius * 1.414213562));
	
	IppiSize sizePatch = {2*radius + 1, 2*radius + 1}; 
	IppiSize sizePatchLarge = {2*radiusLarge+1, 2*radiusLarge+1};

	// CIPPImage<Ipp8u> result(sizePatch, _nChannels, 0);
	// not initialized if failure
	CIPPImage<Ipp8u> result;

	IppiPoint start;
	start.x = rotationAxis.x - radiusLarge;
	start.y = rotationAxis.y - radiusLarge;

	if(start.x >= 0 && start.y >= 0 && 
		start.x + sizePatchLarge.width <= _size.width && 
		start.y + sizePatchLarge.height <= _size.height)
	{
		CIPPImage<Ipp8u> patchLarge = getRegion(start, sizePatchLarge);

		double xShift, yShift;
		CIPPImage<Ipp8u> patchLargeRotated = patchLarge.rotateAroundCenter(angleInDegree, 
			&xShift, &yShift, IPPI_INTER_CUBIC);

		int heightLarge = patchLargeRotated._size.height;
		int widthLarge = patchLargeRotated._size.width;

		IppiPoint centerPatchLarge;
		centerPatchLarge.x = (int)ceil(double(heightLarge)/2);
		centerPatchLarge.y = (int)ceil(double(widthLarge)/2);
			
		IppiPoint newStart;
		newStart.x = centerPatchLarge.x - radius - 1;
		newStart.y = centerPatchLarge.y - radius - 1;

		result = patchLargeRotated.getRegion(newStart, sizePatch);
	}

	return result;
}


/**
 * interface for ippiMirror: 8u_C1R, 8u_C3R, 8u_AC4R\n
 * Flip Specifies the axis to mirror the image about.
 *
 * @param IppiAxis Use the following values to specify the axes:
 *	      axsHorizontal, for the horizontal axis
 *	      axsVertical, for the vertical axis
 *	      axsBoth, for both horizontal and vertical axes.
 * @return the mirror image of the current CIPPImage object
 */
template<>
CIPPImage<Ipp8u> CIPPImage<Ipp8u>::mirror(IppiAxis flip) const
{
	CIPPImage<Ipp8u> result(_size, _nChannels);
	switch(_nChannels)
	{
	case 1:
		ippiMirror_8u_C1R(_imageData, _size.width, result._imageData, _size.width, _size, flip);
		break;
	case 3:
		ippiMirror_8u_C3R(_imageData, _size.width, result._imageData, _size.width, _size, flip);
		break;
	case 4:
		ippiMirror_8u_AC4R(_imageData, _size.width, result._imageData, _size.width, _size, flip);
		break;
	}
	return result;
}


/**
 * interface for ippiCountInRange: 8u_C1R\n
 *
 * This function
 * computes the number of pixels in the image which have intensity values in the range
 * between lowerBound and upperBound (inclusive).
 * In case of a multi-channel image, pixels are counted within intensity range for each
 * color channel separately, and the array counts of three resulting values is returned.
 * The alpha channel values, if present, are not processed.
 * 
 * @param start start pixel position of the ROI
 * @param sizeRegion size of the ROI
 * @param lowerBound lower limit of the intensity range
 * @param upperBound upper limit of the intensity range
 * @return int the number of pixels within the given intensity range.
 */
template<>
int CIPPImage<Ipp8u>::countInRange(IppiPoint start, IppiSize sizeRegion, Ipp8u lowerBound, Ipp8u upperBound) const
{
	int counts;

	int offset = start.x + start.y * _size.width;
	ippiCountInRange_8u_C1R(_imageData + offset, _size.width, sizeRegion, &counts, lowerBound, upperBound);

	return counts;
}

template<>
int CIPPImage<Ipp8u>::countInRange(Ipp8u lowerBound, Ipp8u upperBound) const
{
	int counts;
	ippiCountInRange_8u_C1R(_imageData, _size.width, _size, &counts, lowerBound, upperBound);
	return counts;
}

/**
 * interface for ippiRemap: 8u_C1R\n
 *
 * This function transforms the source image by remapping its pixels. 
 * Pixel remapping is performed using xMap and
 * yMap buffers to look-up the coordinates of the source image pixel that is written to the
 * target destination image pixel. Your application have to supply these look-up tables.
 * The remapping of source pixels to destination pixels is made according to the
 * following formula:\n
 *
 *          dst_pixel[i,j] = src_pixel[xMap[i,j], yMap[i,j]]\n
 *
 * where i,j are the x- and y-coordinates of the target destination image pixel dst_pixel
 * xMap[i,j] contains the x- coordinate of the source image pixel
 * src_pixel that should be written to dst_pixel
 * yMap[i,j] contains the y- coordinate of the source image pixel
 * src_pixel that should be written to dst_pixel
 *
 *
 * @param source source image
 * @param xMap, yMap Pointers to the starts of 2D buffers, containing tables of the
 *		  x- and y-coordinates. If the referenced coordinates correspond to a pixel 
 *        outside of the source ROI, no mapping of the source pixel is done.
 * @param resultSize size of the returning image object
 * @param interpolation The type of interpolation to perform for image resampling.
 *        Use one of the following values:\n
 *        IPP_INTER_NN nearest neighbor interpolation\n
 *        IPP_INTER_LINEAR linear interpolation\n
 *        IPP_INTER_CUBIC cubic interpolation\n
 */
template<>
CIPPImage<Ipp8u> remap(const CIPPImage<Ipp8u> &source, 
					   const CIPPImage<Ipp32f> &xMap, const CIPPImage<Ipp32f> &yMap,
					   IppiSize resultSize, int interpolation)
{
	IppStatus status;

	IppiSize size = source._size;
	int nChannels = source._nChannels;
	CIPPImage<Ipp8u> result(resultSize, source._nChannels, 0);

	IppiRect ROI;
	ROI.x = 0;
	ROI.y = 0;
	ROI.height = size.height;
	ROI.width = size.width;

	// TODO:HOWARD
	// There is a hidden constraint in the ippRemap function
	// that requires xMap and yMap to be continuous field
	// otherwise, the interpolation won't work in the case of singular value
	// and the program status is unpredictable.
	if(1 == nChannels)
	{
		status = ippiRemap_8u_C1R(source._imageData, size, size.width, ROI,
			xMap._imageData, resultSize.width*4,
			yMap._imageData, resultSize.width*4,
			result._imageData, resultSize.width * nChannels, resultSize, IPPI_INTER_CUBIC);
	}
	else if(3 == nChannels)
	{
		status = ippiRemap_8u_C3R(source._imageData, size, size.width * nChannels, ROI,
			xMap._imageData, resultSize.width*4,
			yMap._imageData, resultSize.width*4,
			result._imageData, resultSize.width * nChannels, resultSize, IPPI_INTER_CUBIC);
	}
	else // if(4 == source._nChannels)
	{
	}

	//return CIPPImage<Ipp8u>(result);
	if (status)
		displayStatus(status);
	return result;
}


template<>
CIPPImage<Ipp8u> CIPPImage<Ipp8u>::remap(const CIPPImage<Ipp32f> &xMap, const CIPPImage<Ipp32f> &yMap,
					   IppiSize resultSize, int interpolation) const
{
	IppStatus status;

	CIPPImage<Ipp8u> result(resultSize, _nChannels, 0);

	IppiRect ROI;
	ROI.x = 0;
	ROI.y = 0;
	ROI.height = _size.height;
	ROI.width = _size.width;

	// TODO:HOWARD
	// There is a hidden constraint in the ippRemap function
	// that requires xMap and yMap to be continuous field
	// otherwise, the interpolation won't work in the case of singular value
	// and the program status is unpredictable.
	if(1 == _nChannels)
	{
		status = ippiRemap_8u_C1R(_imageData, _size, _size.width*_nChannels, ROI,
			xMap._imageData, resultSize.width*4,
			yMap._imageData, resultSize.width*4,
			result._imageData, resultSize.width *_nChannels, resultSize, IPPI_INTER_CUBIC);
	}
	else if(3 == _nChannels)
	{
		status = ippiRemap_8u_C3R(_imageData, _size, _size.width *_nChannels, ROI,
			xMap._imageData, resultSize.width*4,
			yMap._imageData, resultSize.width*4,
			result._imageData, resultSize.width * _nChannels, resultSize, IPPI_INTER_CUBIC);
	}
	else // if(4 == _nChannels)
	{
	}


	//return CIPPImage<Ipp32f>(result);
	if (status)
		displayStatus(status);
	return result;
}

// ========================================================
// 14. Computer Vision
// ------------------------------------------------------
/**
 * Calculates distance to closest zero pixel for all non-zero pixels of source iamge.
 *
 * @description
 * Distance transform is used for calculating the distance to an object. The input is an image
 * with feature and non-feature pixels. The function labels every non-feature pixel in the
 * output image with a distance to the closest feature pixel. Feature pixels are marked with zero.
 *
 * Distance transform is used for a wide variety of subjects including skeleton finding and
 * shape analysis.
 *
 * @param kerSize Specifies the mask size as follows: 3 for 3x3 mask, 5 for 5x5 mask.
 * @param norm Specifies the type of matrics. Possible values are:
 *                ippiNormInf(0)  delta = max(|x1-x2|, |y1-y2|)
 *                ippiNormL1(1)   delta = |x1-x2| + |y1-y2|
 *                ippiNormL2(2)   delta = sqrt((x1-x2)^2 + (y1-y2)^2)
 * @return CIPPImage<Ipp32f>
 *
 **/
template<>
CIPPImage<Ipp32f> distanceTransform(const CIPPImage<Ipp8u> &src, IppiNorm norm, int kerSize)
{
	assert(3 == kerSize || 5 == kerSize);
	assert(src.isInitialized() && 1 == src._nChannels);
	
	IppStatus status = ippStsNoErr;
	CIPPImage<Ipp32f> result_32f(src._size, 1, 0);
	if(3 == kerSize)
	{
		Ipp32f metrics[2];	// 2 for 3x3 mask
		// returns an optimal mask for a given type of metrics and given mask size
		status = ippiGetDistanceTransformMask_32f(kerSize, norm, metrics);
		
		//Calculates distance to closest zero pixel for all	non-zero pixels of source image.
		status = ippiDistanceTransform_3x3_8u32f_C1R(src._imageData, src._size.width,
											   result_32f._imageData, src._size.width*4,
											   src._size, metrics);
	}
	else if(5 == kerSize)
	{
		// returns an optimal mask for a given type of metrics and given mask size
		Ipp32f metrics[3];	// 2 for 3x3 mask
		// returns an optimal mask for a given type of metrics and given mask size
		status = ippiGetDistanceTransformMask_32f(kerSize, norm, metrics);

		//Calculates distance to closest zero pixel for all	non-zero pixels of source image.
		status = ippiDistanceTransform_5x5_8u32f_C1R(src._imageData, src._size.width,
											   result_32f._imageData, src._size.width*4,
											   src._size, metrics);
	}
	else
	{
		cout << "\nError -- [distanceTransform] invalid kerSize.";
	}
	if (status)
		displayStatus(status);
	return result_32f;
}

template<>
CIPPImage<Ipp32f> distanceTransformGuassianEuclidean(const CIPPImage<Ipp8u> &src, float sigma, int kerSize)
{
	assert(sigma > 0);
	assert(src.isInitialized() && 1 == src._nChannels);

	// first get the euclidean distance transform
	CIPPImage<Ipp32f> result = distanceTransform(src, ippiNormL2, kerSize);

	float sigma2 = sigma*sigma;
	Ipp32f *p = result._imageData;

	// im_gedt = exp(-im_edt.*im_edt/sigma2);
	// for each pixel do gaussian
	for(int i = 0; i < result._size.width * result._size.height; i++, p++)
		*p = exp(-(*p)*(*p)/sigma2);
	return result;
}

/**
 * Calculates distance to closest zero pixel for all non-zero pixels of source iamge.
 *
 * @description
 * Distance transform is used for calculating the distance to an object. The input is an image
 * with feature and non-feature pixels. The function labels every non-feature pixel in the
 * output image with a distance to the closest feature pixel. Feature pixels are marked with zero.
 *
 * Distance transform is used for a wide variety of subjects including skeleton finding and
 * shape analysis.
 *
 *
 * @param kerSize Specifies the mask size as follows: 3 for 3x3 mask, 5 for 5x5 mask.
 * @param norm Specifies the type of matrics. Possible values are:
 *              
 *        DTType                 IppiNorm         formula
 *        -------------          --------------   -----------------------------------
 *	      DT_CHESSBOARD          ippiNormInf(0)   delta = max(|x1-x2|, |y1-y2|)
 *        DT_CITYBLOCK           ippiNormL1(1)    delta = |x1-x2| + |y1-y2|
 *        DT_EUCLIDEAN           ippiNormL2(2)    delta = sqrt((x1-x2)^2 + (y1-y2)^2)
 *        DT_GAUSSIAN_EUCLIDEAN  N/A              N/A
 *
 * @return CIPPImage<Ipp32f>
 *
 **/
template<>
CIPPImage<Ipp32f> distanceTransformGeneral(const CIPPImage<Ipp8u> &src, int DTType, int kerSize, float sigma)
{
	assert(3 == kerSize || 5 == kerSize);
	assert(src.isInitialized() && 1 == src._nChannels);

	switch(DTType)
	{
	case DT_CHESSBOARD:
	case DT_CITYBLOCK:
	case DT_EUCLIDEAN:
		return distanceTransform(src, IppiNorm(DTType), kerSize);
		break;
	case DT_GAUSSIAN_EUCLIDEAN:
		return distanceTransformGuassianEuclidean(src, sigma, kerSize);
		break;
	default:
		cout << "\nError -- [distanceTransformGeneral] invlid type (" << DTType << ")." << endl;  
		return CIPPImage<Ipp32f>(src._size, 1, 0);
	}
}



/**
 * this function composites two image buffers using either the opacity
 * (alpha) channel in the images or provided alpha values.
 *		OVER	ippAlphaOver
 *		IN		ippAlphaIn
 *		OUT		ippAlphaOut
 *		ATOP	ippAlphaATop
 *		XOR		ippAlphaXor
 *		PLUS	ippAlphaPlus
 **/
template<>
CIPPImage<Ipp8u> alphaComp(const CIPPImage<Ipp8u> &A, const CIPPImage<Ipp8u> &B, IppiAlphaType alphaType)
{
	assert(A._size.width == B._size.width && A._size.height == B._size.height &&
		A._nChannels == B._nChannels);

	IppiSize size = A._size;
	int nChannels = A._nChannels;

	CIPPImage<Ipp8u> result(size, nChannels);

	ippiAlphaComp_8u_AC1R(A._imageData, size.width,
		B._imageData, size.width,
		result._imageData, size.width, size,
		alphaType);
	
	return result;
}



CIPPImage<Ipp16s> convert_8u16s(const CIPPImage<Ipp8u> &image)
{
	int nChannels = image._nChannels;
	IppiSize size = image._size;

	CIPPImage<Ipp16s> result(size, nChannels);

	if(nChannels == 1)
		ippiConvert_8u16s_C1R( image._imageData, size.width, result._imageData, size.width * 2, size);
	else if(nChannels == 3)
		ippiConvert_8u16s_C3R( image._imageData, size.width* image._nChannels, 
			result._imageData, size.width * image._nChannels* 2, size);
	else // if(img->nChannels == 4)
		ippiConvert_8u16s_AC4R( image._imageData, size.width* image._nChannels, 
			result._imageData, size.width * image._nChannels* 2, size);
	return result;
}

CIPPImage<Ipp16u> convert_8u16u(const CIPPImage<Ipp8u> &image)
{
	int nChannels = image._nChannels;
	IppiSize size = image._size;

	CIPPImage<Ipp16u> result(size, nChannels);

	if(nChannels == 1)
		ippiConvert_8u16u_C1R( image._imageData, size.width, 
			result._imageData, size.width * 2, size);
	else if(nChannels == 3)
		ippiConvert_8u16u_C3R( image._imageData, size.width* image._nChannels, 
			result._imageData, size.width * image._nChannels* 2, size);
	else // if(img->nChannels == 4)
		ippiConvert_8u16u_AC4R( image._imageData, size.width* image._nChannels, 
			result._imageData, size.width * image._nChannels* 2, size);
	return result;
}


CIPPImage<Ipp32f> convert_8u32f(const CIPPImage<Ipp8u> &image)
{
	int nChannels = image._nChannels;
	IppiSize size = image._size;

	CIPPImage<Ipp32f> result(size, nChannels);

	if(nChannels == 1)
		ippiConvert_8u32f_C1R( image._imageData, size.width, 
			result._imageData, size.width * 4, size);
	else if(nChannels == 3)
		ippiConvert_8u32f_C3R( image._imageData, size.width* image._nChannels, 
			result._imageData, size.width*image._nChannels*4, size);
	else // if(img->nChannels == 4)
		ippiConvert_8u32f_AC4R( image._imageData, size.width* image._nChannels, 
			result._imageData, size.width*image._nChannels*4, size);
	return result;
}

CIPPImage<Ipp32f> convert_16s32f(const CIPPImage<Ipp16s> &image)
{
	int nChannels = image._nChannels;
	IppiSize size = image._size;

	CIPPImage<Ipp32f> result(size, nChannels);

	if(nChannels == 1)
		ippiConvert_16s32f_C1R( image._imageData, size.width*2, 
			result._imageData, size.width * 4, size);
	else if(nChannels == 3)
		ippiConvert_16s32f_C3R( image._imageData, size.width* image._nChannels*2, 
			result._imageData, size.width*image._nChannels*4, size);
	else // if(img->nChannels == 4)
		ippiConvert_16s32f_AC4R( image._imageData, size.width* image._nChannels*2, 
			result._imageData, size.width*image._nChannels*4, size);

	return result;
}

CIPPImage<Ipp32f> convert_16u32f(const CIPPImage<Ipp16u> &image)
{
	int nChannels = image._nChannels;
	IppiSize size = image._size;

	CIPPImage<Ipp32f> result(size, nChannels);

	if(nChannels == 1)
		ippiConvert_16u32f_C1R( image._imageData, size.width*2, 
			result._imageData, size.width * 4, size);
	else if(nChannels == 3)
		ippiConvert_16u32f_C3R( image._imageData, size.width* image._nChannels*2, 
			result._imageData, size.width*image._nChannels*4, size);
	else // if(img->nChannels == 4)
		ippiConvert_16u32f_AC4R( image._imageData, size.width* image._nChannels*2, 
			result._imageData, size.width*image._nChannels*4, size);
	return result;
}


CIPPImage<Ipp8u> convert_16s8u(const CIPPImage<Ipp16s> &image)
{
	int nChannels = image._nChannels;
	IppiSize size = image._size;

	CIPPImage<Ipp8u> result(size, nChannels);

	if(nChannels == 1)
		ippiConvert_16s8u_C1R( image._imageData, size.width*2, 
			result._imageData, size.width, size);
	else if(nChannels == 3)
		ippiConvert_16s8u_C3R( image._imageData, size.width*image._nChannels*2, 
			result._imageData, size.width*image._nChannels, size);
	else // if(img->nChannels == 4)
		ippiConvert_16s8u_AC4R( image._imageData, size.width*image._nChannels*2, 
			result._imageData, size.width*image._nChannels, size);
	return result;
}



CIPPImage<Ipp8u> convert_16u8u(const CIPPImage<Ipp16u> &image)
{
	int nChannels = image._nChannels;
	IppiSize size = image._size;

	CIPPImage<Ipp8u> result(size, nChannels);

	if(nChannels == 1)
		ippiConvert_16u8u_C1R( image._imageData, size.width*2, 
			result._imageData, size.width, size);
	else if(nChannels == 3)
		ippiConvert_16u8u_C3R( image._imageData, size.width*image._nChannels*2, 
			result._imageData, size.width*image._nChannels, size);
	else // if(img->nChannels == 4)
		ippiConvert_16u8u_AC4R( image._imageData, size.width*image._nChannels*2, 
			result._imageData, size.width*image._nChannels, size);
	return result;
}



//roundMode Rounding mode which can be ippRndZero or ippRndNear:
//ippRndZero Specifies that floating-point values must
//be truncated toward zero.
//ippRndNear Specifies that floating-point values must
//be rounded to the nearest integer.
CIPPImage<Ipp8u> convert_32f8u(const CIPPImage<Ipp32f> &image, IppRoundMode roundMode)
{
	int nChannels = image._nChannels;
	IppiSize size = image._size;

	CIPPImage<Ipp8u> result(size, nChannels);

	if(nChannels == 1)
		ippiConvert_32f8u_C1R( image._imageData, size.width*4, 
			result._imageData, size.width, size, roundMode);
	else if(nChannels == 3)
		ippiConvert_32f8u_C3R( image._imageData, size.width*image._nChannels*4, 
			result._imageData, size.width*image._nChannels, size, roundMode);
	else // if(img->nChannels == 4)
		ippiConvert_32f8u_AC4R( image._imageData, size.width*image._nChannels*4, 
			result._imageData, size.width *image._nChannels, size, roundMode);
	return result;
}


CIPPImage<Ipp8u> scale_32f8u(const CIPPImage<Ipp32f> &image32f, IppRoundMode roundMode)
{
	int nChannels = image32f._nChannels;
	IppiSize size = image32f._size;
	CIPPImage<Ipp8u> image8u(size, nChannels);
	IppStatus status;
	Ipp32f min1, max1;
	//int stepBytes;
	switch(nChannels)
	{
	case 1:
		// get the max and min of the image
		// rescale the image so we can display it
		ippiMin_32f_C1R(image32f._imageData, size.width * 4 * nChannels, size, &min1);
		ippiMax_32f_C1R(image32f._imageData, size.width * 4 * nChannels, size, &max1);

		// if  max and min are the same, that means this image has a constant
		// value (max == min), therefore, we don't need to do the more expensive
		// scale operation. A single set operation would surfice.
		if(max1 == min1)
			image8u.set(Ipp8u(min1));
		else	// scale img into tmp
			status = ippiScale_32f8u_C1R(image32f._imageData, size.width * 4, image8u._imageData, size.width, size, min1, max1);
		break;
	case 3:
		// HOWARD : TODO
		// find min and max of each channel, pick the max(max(channel)) and min(min(channel)) and use thse
		// to scale.
		Ipp32f min3[3];
		Ipp32f max3[3];
		ippiMin_32f_C3R(image32f._imageData, size.width*4*nChannels, size, min3);
		ippiMax_32f_C3R(image32f._imageData, size.width*4*nChannels, size, max3);

		min1 = min(min(min3[0], min3[1]), min3[2]);
		max1 = max(max(max3[0], max3[1]), max3[2]);

		if(max1 == min1)
			image8u.set(Ipp8u(min1));
		else
			status = ippiScale_32f8u_C3R(image32f._imageData, size.width * 4 * nChannels, image8u._imageData, size.width * nChannels, size, min1, max1); 
		break;
	case 4:
		Ipp32f min4[4];
		Ipp32f max4[4];
		ippiMin_32f_AC4R(image32f._imageData, size.width*4*nChannels, size, min4);
		ippiMax_32f_AC4R(image32f._imageData, size.width*4*nChannels, size, max4);

		min1 = min(min(min4[0], min4[1]),min(min4[2], min4[3]));
		max1 = max(max(max4[0], max4[1]),max(max4[2], max4[3]));

		if(max1 == min1)
			image8u.set(Ipp8u(min1));
		else
			status = ippiScale_32f8u_AC4R(image32f._imageData, size.width * 4 * nChannels, image8u._imageData, size.width * nChannels, size, min1, max1); 
		break;
	default:
		printf("\nError -- [IPPImage_8u.cpp::convert_32f8uScaled] nChannels can only be 1, 3, or 4");
		return image8u;
	}
	if (status)
		displayStatus(status);
	return image8u;
}

CIPPImage<Ipp16s> convert_32f16s(const CIPPImage<Ipp32f> &image, IppRoundMode roundMode)
{
	int nChannels = image._nChannels;
	IppiSize size = image._size;

	CIPPImage<Ipp16s> result(size, nChannels);

	if(nChannels == 1)
		ippiConvert_32f16s_C1R( image._imageData, size.width*4, 
			result._imageData, size.width*2, size, roundMode);
	else if(nChannels == 3)
		ippiConvert_32f16s_C3R( image._imageData, size.width*image._nChannels*4, 
			result._imageData, size.width*image._nChannels*2, size, roundMode);
	else // if(img->nChannels == 4)
		ippiConvert_32f16s_AC4R( image._imageData, size.width*image._nChannels*4, 
			result._imageData, size.width *image._nChannels*2, size, roundMode);
	return result;
}

CIPPImage<Ipp16u> convert_32f16u(const CIPPImage<Ipp32f> &image, IppRoundMode roundMode)
{
	int nChannels = image._nChannels;
	IppiSize size = image._size;

	CIPPImage<Ipp16u> result(size, nChannels);

	if(nChannels == 1)
		ippiConvert_32f16u_C1R( image._imageData, size.width*4, 
			result._imageData, size.width*2, size, roundMode);
	else if(nChannels == 3)
		ippiConvert_32f16u_C3R( image._imageData, size.width*image._nChannels*4, 
			result._imageData, size.width*image._nChannels*2, size, roundMode);
	else // if(img->nChannels == 4)
		ippiConvert_32f16u_AC4R( image._imageData, size.width*image._nChannels*4, 
			result._imageData, size.width *image._nChannels*2, size, roundMode);
	return result;
}

// ========================================================
// fast convesion
// using reference to pass object instead of returning
// but needs to set up memory for the result outside the
// function call
// --------------------------------------------------------
void convert_8u16s(const CIPPImage<Ipp8u> &image, CIPPImage<Ipp16s> &result)
{
	assert(image._imageData != NULL && result._imageData != NULL);
	int nChannels = image._nChannels;
	IppiSize size = image._size;

	if(nChannels == 1)
		ippiConvert_8u16s_C1R( image._imageData, size.width, result._imageData, size.width * 2, size);
	else if(nChannels == 3)
		ippiConvert_8u16s_C3R( image._imageData, size.width* image._nChannels, 
			result._imageData, size.width * image._nChannels* 2, size);
	else // if(img->nChannels == 4)
		ippiConvert_8u16s_AC4R( image._imageData, size.width* image._nChannels, 
			result._imageData, size.width * image._nChannels* 2, size);
}


void convert_8u16u(const CIPPImage<Ipp8u> &image, CIPPImage<Ipp16u> &result)
{
	assert(image._imageData != NULL && result._imageData != NULL);
	int nChannels = image._nChannels;
	IppiSize size = image._size;

	if(nChannels == 1)
		ippiConvert_8u16u_C1R( image._imageData, size.width, 
			result._imageData, size.width * 2, size);
	else if(nChannels == 3)
		ippiConvert_8u16u_C3R( image._imageData, size.width* image._nChannels, 
			result._imageData, size.width * image._nChannels* 2, size);
	else // if(img->nChannels == 4)
		ippiConvert_8u16u_AC4R( image._imageData, size.width* image._nChannels, 
			result._imageData, size.width * image._nChannels* 2, size);
}

void convert_8u32f	(const CIPPImage<Ipp8u>	&image, CIPPImage<Ipp32f>&result)
{
	assert(image._imageData != NULL && result._imageData != NULL);
	int nChannels = image._nChannels;
	IppiSize size = image._size;

	if(nChannels == 1)
		ippiConvert_8u32f_C1R( image._imageData, size.width, 
			result._imageData, size.width * 4, size);
	else if(nChannels == 3)
		ippiConvert_8u32f_C3R( image._imageData, size.width* image._nChannels, 
			result._imageData, size.width*image._nChannels*4, size);
	else // if(img->nChannels == 4)
		ippiConvert_8u32f_AC4R( image._imageData, size.width* image._nChannels, 
			result._imageData, size.width*image._nChannels*4, size);
}


void convert_16s32f	(const CIPPImage<Ipp16s>	&image, CIPPImage<Ipp32f>	&result)
{
	assert(image._imageData != NULL && result._imageData != NULL);
	int nChannels = image._nChannels;
	IppiSize size = image._size;

	if(nChannels == 1)
		ippiConvert_16s32f_C1R( image._imageData, size.width*2, 
			result._imageData, size.width * 4, size);
	else if(nChannels == 3)
		ippiConvert_16s32f_C3R( image._imageData, size.width* image._nChannels*2, 
			result._imageData, size.width*image._nChannels*4, size);
	else // if(img->nChannels == 4)
		ippiConvert_16s32f_AC4R( image._imageData, size.width* image._nChannels*2, 
			result._imageData, size.width*image._nChannels*4, size);
}

void convert_16u32f	(const CIPPImage<Ipp16u>	&image, CIPPImage<Ipp32f>	&result)
{
	assert(image._imageData != NULL && result._imageData != NULL);
	int nChannels = image._nChannels;
	IppiSize size = image._size;
	if(nChannels == 1)
		ippiConvert_16u32f_C1R( image._imageData, size.width*2, 
			result._imageData, size.width * 4, size);
	else if(nChannels == 3)
		ippiConvert_16u32f_C3R( image._imageData, size.width* image._nChannels*2, 
			result._imageData, size.width*image._nChannels*4, size);
	else // if(img->nChannels == 4)
		ippiConvert_16u32f_AC4R( image._imageData, size.width* image._nChannels*2, 
			result._imageData, size.width*image._nChannels*4, size);
}


void convert_16s8u	(const CIPPImage<Ipp16s>	&image, CIPPImage<Ipp8u>	&result)
{
	assert(image._imageData != NULL && result._imageData != NULL);
	int nChannels = image._nChannels;
	IppiSize size = image._size;
	if(nChannels == 1)
		ippiConvert_16s8u_C1R( image._imageData, size.width*2, 
			result._imageData, size.width, size);
	else if(nChannels == 3)
		ippiConvert_16s8u_C3R( image._imageData, size.width*image._nChannels*2, 
			result._imageData, size.width*image._nChannels, size);
	else // if(img->nChannels == 4)
		ippiConvert_16s8u_AC4R( image._imageData, size.width*image._nChannels*2, 
			result._imageData, size.width*image._nChannels, size);
}

void convert_16u8u	(const CIPPImage<Ipp16u>	&image, CIPPImage<Ipp8u>	&result)
{
	assert(image._imageData != NULL && result._imageData != NULL);
	int nChannels = image._nChannels;
	IppiSize size = image._size;

	if(nChannels == 1)
		ippiConvert_16u8u_C1R( image._imageData, size.width*2, 
			result._imageData, size.width, size);
	else if(nChannels == 3)
		ippiConvert_16u8u_C3R( image._imageData, size.width*image._nChannels*2, 
			result._imageData, size.width*image._nChannels, size);
	else // if(img->nChannels == 4)
		ippiConvert_16u8u_AC4R( image._imageData, size.width*image._nChannels*2, 
			result._imageData, size.width*image._nChannels, size);
}

//roundMode Rounding mode which can be ippRndZero or ippRndNear:
//ippRndZero Specifies that floating-point values must
//be truncated toward zero.
//ippRndNear Specifies that floating-point values must
//be rounded to the nearest integer.
void convert_32f8u	(const CIPPImage<Ipp32f>	&image, CIPPImage<Ipp8u>	&result, IppRoundMode roundMode)
{
	assert(image._imageData != NULL && result._imageData != NULL);
	int nChannels = image._nChannels;
	IppiSize size = image._size;

	if(nChannels == 1)
		ippiConvert_32f8u_C1R( image._imageData, size.width*4, 
			result._imageData, size.width, size, roundMode);
	else if(nChannels == 3)
		ippiConvert_32f8u_C3R( image._imageData, size.width*image._nChannels*4, 
			result._imageData, size.width*image._nChannels, size, roundMode);
	else // if(img->nChannels == 4)
		ippiConvert_32f8u_AC4R( image._imageData, size.width*image._nChannels*4, 
			result._imageData, size.width *image._nChannels, size, roundMode);
}

void convert_32f16s	(const CIPPImage<Ipp32f>	&image, CIPPImage<Ipp16s>	&result, IppRoundMode roundMode)
{
	assert(image._imageData != NULL && result._imageData != NULL);
	int nChannels = image._nChannels;
	IppiSize size = image._size;
	if(nChannels == 1)
		ippiConvert_32f16s_C1R( image._imageData, size.width*4, 
			result._imageData, size.width*2, size, roundMode);
	else if(nChannels == 3)
		ippiConvert_32f16s_C3R( image._imageData, size.width*image._nChannels*4, 
			result._imageData, size.width*image._nChannels*2, size, roundMode);
	else // if(img->nChannels == 4)
		ippiConvert_32f16s_AC4R( image._imageData, size.width*image._nChannels*4, 
			result._imageData, size.width *image._nChannels*2, size, roundMode);
}

void convert_32f16u	(const CIPPImage<Ipp32f>	&image, CIPPImage<Ipp16u>	&result, IppRoundMode roundMode)
{
	assert(image._imageData != NULL && result._imageData != NULL);
	int nChannels = image._nChannels;
	IppiSize size = image._size;

	if(nChannels == 1)
		ippiConvert_32f16u_C1R( image._imageData, size.width*4, 
			result._imageData, size.width*2, size, roundMode);
	else if(nChannels == 3)
		ippiConvert_32f16u_C3R( image._imageData, size.width*image._nChannels*4, 
			result._imageData, size.width*image._nChannels*2, size, roundMode);
	else // if(img->nChannels == 4)
		ippiConvert_32f16u_AC4R( image._imageData, size.width*image._nChannels*4, 
			result._imageData, size.width *image._nChannels*2, size, roundMode);
}


template<>
CIPPImage<Ipp8u> operator* (const CIPPImage<Ipp8u> &image1, const CIPPImage<Ipp8u> &image2)
{
	assert(image1._size.width == image2._size.width && image1._size.height == image2._size.height
		&& image1._nChannels == image2._nChannels);

	IppiSize size = image1._size;

	CIPPImage<Ipp8u> result(image1._size, image1._nChannels);
	ippiMul_8u_C1RSfs(result._imageData, size.width, image1._imageData, size.width, 
			image2._imageData, size.width, size, 0);

	return result;
}

// take the highest value of the image and subtract the entire image
template<>
CIPPImage<Ipp8u> CIPPImage<Ipp8u>::invert() const
{
	CIPPImage<Ipp8u> result(_size, _nChannels, 0);
	Ipp8u max;

	// get the max and min of the image
	// rescale the image so we can display it
	// ippiMin_8u_C1R(image, _size.width, _size, &min);
	ippiMax_8u_C1R(_imageData, _size.width, _size, &max);
	ippiAbsDiffC_8u_C1R(_imageData, _size.width, result._imageData, _size.width, _size, max);

	return result;
}

template<>
CIPPImage<Ipp8u> CIPPImage<Ipp8u>::invertBinaryMask() const
{
	assert(_nChannels == 1);
	CIPPImage<Ipp8u> mask(_size, _nChannels, Ipp8u(1));
	ippiSet_8u_C1MR(0, mask._imageData, mask._size.width,
		_size, _imageData, _size.width);
	return mask;
}



template<>
CIPPImage<Ipp8u> CIPPImage<Ipp8u>::RGBToLUV() const
{
	if(1 == _nChannels) return (*this);

	CIPPImage<Ipp8u> result(_size, _nChannels);
	if(3 == _nChannels)
		ippiRGBToLUV_8u_C3R(_imageData, 
			_size.width*_nChannels,
			result._imageData,
			result._size.width*_nChannels,
			_size);
	else // if(4 == _nChannels)
		ippiRGBToLUV_8u_AC4R(_imageData, 
			_size.width*_nChannels,
			result._imageData,
			result._size.width*_nChannels,
			_size);

	return result;
}
	
template<>
CIPPImage<Ipp8u> CIPPImage<Ipp8u>::LUVToRGB() const
{
	if(1 == _nChannels) return (*this);

	CIPPImage<Ipp8u> result(_size, _nChannels);
	if(3 == _nChannels)
		ippiLUVToRGB_8u_C3R(_imageData, 
			_size.width*_nChannels,
			result._imageData,
			result._size.width*_nChannels,
			_size);
	else // if(4 == _nChannels)
		ippiLUVToRGB_8u_AC4R(_imageData, 
			_size.width*_nChannels,
			result._imageData,
			result._size.width*_nChannels,
			_size);
	
	return result;
}


template<>
CIPPImage<Ipp8u> CIPPImage<Ipp8u>::BGRToLab(void) const
{
	if(3 != _nChannels) return (*this);

	CIPPImage<Ipp8u> result(_size, _nChannels);
	ippiBGRToLab_8u_C3R(_imageData, 
		_size.width*_nChannels,
		result._imageData,
		result._size.width*_nChannels,
		_size);

	return result;
}
	
template<>
CIPPImage<Ipp8u> CIPPImage<Ipp8u>::LabToBGR(void) const
{
	if(3 != _nChannels) return (*this);

	CIPPImage<Ipp8u> result(_size, _nChannels);
	ippiLabToBGR_8u_C3R(_imageData, 
		_size.width*_nChannels,
		result._imageData,
		result._size.width*_nChannels,
		_size);
	
	return result;
}

template<>
CIPPImage<Ipp8u> CIPPImage<Ipp8u>::RGBToHSV(void) const
{
	CIPPImage<Ipp8u> result(_size, _nChannels);

	switch (_nChannels) {
	case 1:
		return *this;
	case 3:
		ippiRGBToHSV_8u_C3R(_imageData,
			_size.width * _nChannels,
			result._imageData,
			result._size.width * _nChannels,
			_size);
		break;
	case 4:
		ippiRGBToHSV_8u_AC4R(_imageData,
			_size.width * _nChannels,
			result._imageData,
			result._size.width * _nChannels,
			_size);
		break;
	default:
		assert(0);
	}
	return result;
}

template<>
CIPPImage<Ipp8u> CIPPImage<Ipp8u>::RGBToBGR() const
{
	if(3 != _nChannels) return (*this);

	CIPPImage<Ipp8u> result(_size, _nChannels);

	for(int i = 0; i < _size.height*_size.width*3; i+=3)
	{
		result._imageData[i]   = _imageData[i+2];
		result._imageData[i+1] = _imageData[i+1];
		result._imageData[i+2] = _imageData[i];
	}
	return result;
}
	
template<>
CIPPImage<Ipp8u> CIPPImage<Ipp8u>::BGRToRGB() const
{
	if(3 != _nChannels) return (*this);

	CIPPImage<Ipp8u> result(_size, _nChannels);

	for(int i = 0; i < _size.height*_size.width*3; i+=3)
	{
		result._imageData[i]   = _imageData[i+2];
		result._imageData[i+1] = _imageData[i+1];
		result._imageData[i+2] = _imageData[i];
	}
	return result;
}


template<>
CIPPImage<Ipp8u> CIPPImage<Ipp8u>::RGBToGray(void) const
{
	if(1 == _nChannels) return (*this);

	CIPPImage<Ipp8u> result(_size, 1);
	if(3 == _nChannels)
		ippiRGBToGray_8u_C3C1R(_imageData, 
			_size.width*_nChannels,
			result._imageData,
			result._size.width,
			_size);
	else // if(4 == _nChannels)
		ippiRGBToGray_8u_AC4C1R(_imageData, 
			_size.width*_nChannels,
			result._imageData,
			result._size.width,
			_size);

	return result;
}



template<>
CIPPImage<Ipp8u> CIPPImage<Ipp8u>::makeMask(Ipp8u threshold) const
{
	//assert(1==_nChannels);
	CIPPImage<Ipp8u> mask(_size, 1, Ipp8u(0));
	
	if(1 == _nChannels)
	{
		if(threshold == 0)
		{	ippiSet_8u_C1MR(1, mask._imageData, mask._size.width,
				_size, _imageData, _size.width);
		}
		else
		{
			for(int x = 0; x < _size.width; x++)
				for(int y = 0; y < _size.height; y++)
					if(threshold < _imageData[x+y*_size.width])
						mask._imageData[x+y*_size.width] = 1;
		}
	}
	else
	{
		CIPPImage<Ipp8u> imageGray = RGBToGray();
		if(threshold == 0)
		{	ippiSet_8u_C1MR(1, mask._imageData, mask._size.width,
				_size, imageGray._imageData, _size.width);
		}
		else
		{
			for(int x = 0; x < _size.width; x++)
				for(int y = 0; y < _size.height; y++)
					if(threshold < imageGray._imageData[x+y*_size.width])
						mask._imageData[x+y*_size.width] = 1;
		}

	}
	return mask;
}


template<>
IppStatus CIPPImage<Ipp8u>::loadBinary(const char * filename, Ipp8u threshold, int fileType)
{	// force it to be loaded as gray scale
	IppStatus status = load(filename, fileType, 0);
	// make sure the image only has two values
	*this = makeMask(threshold);
	return status;
}


// ------------------------------------------------------
// 8. Morphological Operation
// ------------------------------------------------------
//CIPPImage<T> morphDilate3x3		(void)			const;
//CIPPImage<T> morphErode3x3		(void)			const;
//CIPPImage<T> morphDilate		(const T *pMask, IppiSize maskSize, IppiPoint anchor)	const;
//CIPPImage<T> morphErode			(const T *pMask, IppiSize maskSize, IppiPoint anchor)	const;
//CIPPImage<T> morphTophatBorder	(const T *pMask, IppiSize maskSize, IppiPoint anchor)	const;
//CIPPImage<T> morphBlackhatBorder(const T *pMask, IppiSize maskSize, IppiPoint anchor)	const;
//CIPPImage<T> morphGradientBorder(const T *pMask, IppiSize maskSize, IppiPoint anchor)	const;

//Ipp8u pMask[11*11] = {0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0,
//					0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
//					1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
//					1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
//					1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
//					1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
//					1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
//					1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
//					1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
//					0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
//					0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0};

////Ipp8u pMask[9*9] = { 0, 0, 1, 1, 1, 1, 1, 0, 0,
////					0, 1, 2, 2, 2, 2, 2, 1, 0,
////					1, 2, 2, 3, 3, 3, 2, 2, 1,
////					1, 2, 3, 3, 4, 3, 3, 2, 1,
////					1, 2, 3, 4, 4, 4, 3, 2, 1,
////					1, 2, 3, 3, 4, 3, 3, 2, 1,
////					1, 2, 2, 3, 3, 3, 2, 2, 1,
////					0, 1, 2, 2, 2, 2, 2, 1, 0,
////					0, 0, 1, 1, 1, 1, 1, 0, 0};

//IppiPoint anchor = {5, 5};
//IppiSize maskSize = {11, 11};


template<>
CIPPImage<Ipp8u> CIPPImage<Ipp8u>::morphOpenBorder(const Ipp8u *pMask, IppiSize maskSize, IppiPoint anchor)	const
{
	IppStatus status = ippStsNoErr;
	IppiMorphAdvState* pState = NULL;
	
	CIPPImage<Ipp8u> result(*this);

	// allocate and initialize the morphAdvState
	status = ippiMorphAdvInitAlloc_8u_C1R(&pState, _size, pMask, maskSize, anchor);
	status = ippiMorphOpenBorder_8u_C1R(_imageData, _size.width, result._imageData, _size.width, _size, ippBorderRepl, pState);

	ippiMorphAdvFree(pState);
	pState = NULL;

	if (status)
		displayStatus(status);
	return result;
}

template<>
CIPPImage<Ipp8u> CIPPImage<Ipp8u>::morphCloseBorder(const Ipp8u *pMask, IppiSize maskSize, IppiPoint anchor)	const
{
	IppStatus status = ippStsNoErr;
	IppiMorphAdvState* pState = NULL;
	
	CIPPImage<Ipp8u> result(*this);

	// allocate and initialize the morphAdvState
	status = ippiMorphAdvInitAlloc_8u_C1R(&pState, _size, pMask, maskSize, anchor);
	status = ippiMorphCloseBorder_8u_C1R(_imageData, _size.width, result._imageData, _size.width, _size, ippBorderRepl, pState);

	ippiMorphAdvFree(pState);
	pState = NULL;

	if (status)
		displayStatus(status);
	return result;
}


template<>
CIPPImage<Ipp8u> CIPPImage<Ipp8u>::getChannel(int channel) const
{
	assert(1 < _nChannels);
	assert(0 <= channel && channel < _nChannels);

	CIPPImage<Ipp8u> result(_size, 1);
	//IppStatus status = 
	ippiCopy_8u_C3C1R(_imageData+channel, 
		_size.width*_nChannels, result._imageData, _size.width, _size);
	
	return result;
}


// regional maxima and minima
template<>
CIPPImage<Ipp8u> CIPPImage<Ipp8u>::findPeaks3x3(Ipp8u threshold, vector<IppiPoint> &peaks, int maxPeakCount, IppiNorm norm, int border) const
{
	assert(1 == _nChannels);
	assert(peaks.size() == 0);
	IppStatus status;
	int bufferSize, peakCount;
	
	CIPPImage<Ipp32f> image32f = convert_8u32f(*this);
	IppiPoint origin = {0, 0};

	// pad the image to the step size
	CIPPImage<Ipp32f> imagePad(image32f._stepBytes/4, image32f._size.height, 1, Ipp32f(0));
	imagePad.copyRegion(origin, image32f, origin, image32f._size);

	status = ippiFindPeaks3x3GetBufferSize_32f_C1R(imagePad._size.width, &bufferSize);
	if (status)
		displayStatus(status);

	Ipp8u *pBuffer = new Ipp8u[bufferSize];
	IppiPoint *pPeak = new IppiPoint[maxPeakCount];

	status = ippiFindPeaks3x3_32f_C1R(imagePad._imageData, imagePad._size.width*4, imagePad._size,
									  threshold, pPeak, maxPeakCount, &peakCount,
									  norm, border, pBuffer);
		
	// draw the regional maxima images
	CIPPImage<Ipp8u> peakMask(_size, 1, Ipp8u(0));
	for(int i = 0; i < peakCount; i++)
	{ 
		peaks.push_back(pPeak[i]);
		peakMask.setPixel(pPeak[i].x, pPeak[i].y, 1);
	}

	delete[] pPeak;		pPeak = NULL;
	delete[] pBuffer;	pBuffer = NULL;

	if (status)
		displayStatus(status);
	return peakMask;
}


//Labels markers in image with different values.
//The function ippiLabelMarkers is declared in the ippcv.h It operates with ROI (see
//Regions of Interest in Intel IPP).
//This function labels markers in the image pSrcDst with different integer values. Each
//connected set of non-zero image pixels is treated as the separate marker. 4- or
//8-connectivity can be used depending on the norm type. All pixels belonging to the same
//marker are set to the same value from the interval [minLabel, maxLabel]. Two markers
//can be labeled with the same value if the number of connected components exceeds
//minLabel-maxLabel+1. The image with labeled markers can be used as the seed image
//for segmentation by functions SegmentWatershed or SegmentGradient functions.
//The function requires the working buffer pBuffer whose size should be computed by the
//function LabelMarkersGetBufferSize beforehand.
template<>
CIPPImage<Ipp8u> CIPPImage<Ipp8u>::labelMarkers(int &numLabels, int minLabel, int maxLabel, IppiNorm norm) const
{
	assert(isInitialized());
	assert(1 == _nChannels);
	int bufferSize;
	IppStatus status = ippiLabelMarkersGetBufferSize_8u_C1R(_size, &bufferSize);

	Ipp8u *pBuffer = new Ipp8u[bufferSize];
	CIPPImage<Ipp8u> result(*this);
	status = ippiLabelMarkers_8u_C1IR(result._imageData, _size.width, _size, 
		minLabel, maxLabel, norm, &numLabels, pBuffer);
	delete [] pBuffer;
	if (status)
		displayStatus(status);
	return result;
}
	
template<>
CIPPImage<Ipp8u> CIPPImage<Ipp8u>::removeSmallRegions(int threshold) const
{
	int numLabels;
	CIPPImage<Ipp8u> connectedComponents = labelMarkers(numLabels);

	vector<int> regionSizeList(numLabels+1, 0);
	// calculate the size of the all the connected components
	for(int i = 0; i < _size.width * _size.height; i++)
		regionSizeList[connectedComponents._imageData[i]]++;

	// cc starts at 1, but our list needs to have zero
	bool * removeList = new bool[numLabels + 1];
	for(int i = 0; i <= numLabels; i++)
		removeList[i] = (regionSizeList[i] < threshold);
	removeList[0] = false;

	for(int i = 0; i < _size.width * _size.height; i++)
		if(removeList[connectedComponents._imageData[i]])
			connectedComponents._imageData[i] = 0;
	delete [] removeList;

	return connectedComponents.makeBinary(0); // make bindary
}


void displayStatus(IppStatus status)
{
	switch(status)
	{
	case ippStsNoErr:
		cout << "\nOK -- no error";
		break;
	case ippStsNullPtrErr:
		cout << "\nError -- null pointer error";
		break;
	case ippStsSizeErr:
		cout << "\nError -- size error";
		break;
	case ippStsStepErr:
		cout << "\nError -- step error";
		break;
	case ippStsNotEvenStepErr:
		cout << "\nError -- not even step error. srcStep is not divisible by 4.";
		break;
	case ippStsBadArgErr:
		cout << "\nError -- bad argument error";
		break;
		//Indicates an error condition if one of the minLabel, maxLabel, and norm has an illegal value.
	default:
		cout << "\nError -- invalid status";
	}
}
