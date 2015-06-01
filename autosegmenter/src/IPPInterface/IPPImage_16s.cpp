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
CIPPImage<Ipp16s>::CIPPImage(IppiSize size, int nChannels)
{
	if(nChannels == 1)
		_imageData = ippiMalloc_16s_C1	( size.width, size.height, &_stepBytes);
	else if(nChannels == 3)
		_imageData = ippiMalloc_16s_C3	( size.width, size.height, &_stepBytes);
	else // if(img->nChannels == 4)
		_imageData = ippiMalloc_16s_AC4   ( size.width, size.height, &_stepBytes);

	_size = size;
	_nChannels = nChannels;
}

template<>
CIPPImage<Ipp16s>::CIPPImage(int width, int height, int nChannels )
{

	if(nChannels == 1)
		_imageData = ippiMalloc_16s_C1	( width, height, &_stepBytes);
	else if(nChannels == 3)
		_imageData = ippiMalloc_16s_C3	( width, height, &_stepBytes);
	else // if(img->nChannels == 4)
		_imageData = ippiMalloc_16s_AC4   ( width, height, &_stepBytes);

	_size.width = width;
	_size.height = height;
	_nChannels = nChannels;
}

template<>
CIPPImage<Ipp16s>::CIPPImage(IppiSize size, int nChannels, const Ipp16s value)
{
	_size = size;
	_nChannels = nChannels;

	if(_nChannels == 1)
	{
		_imageData = ippiMalloc_16s_C1	( _size.width, _size.height, &_stepBytes);
		ippiSet_16s_C1R(value, _imageData, _size.width*2, _size);
	}
	else if(_nChannels == 3)
	{
		Ipp16s values[3] = {value, value, value};
		_imageData = ippiMalloc_16s_C3	( _size.width, _size.height, &_stepBytes);
		ippiSet_16s_C3R(values, _imageData, _size.width*2, _size);
	}
	else // if(img->nChannels == 4)
	{
		Ipp16s values[4] = {value, value, value, value};
		_imageData = ippiMalloc_16s_AC4   ( _size.width, _size.height, &_stepBytes);
		ippiSet_16s_AC4R(values, _imageData, _size.width*2, _size);
	}
}


template<>
CIPPImage<Ipp16s>::CIPPImage(int width, int height, int nChannels, const Ipp16s value )
{
	_size.width = width;
	_size.height = height;
	_nChannels = nChannels;

	if(_nChannels == 1)
	{
		_imageData = ippiMalloc_16s_C1	( _size.width, _size.height, &_stepBytes);
		ippiSet_16s_C1R(value, _imageData, _size.width*2, _size);
	}
	else if(_nChannels == 3)
	{
		Ipp16s values[3] = {value, value, value};
		_imageData = ippiMalloc_16s_C3	( _size.width, _size.height, &_stepBytes);
		ippiSet_16s_C3R(values, _imageData, _size.width*2, _size);
	}
	else // if(img->nChannels == 4)
	{
		Ipp16s values[4] = {value, value, value, value};
		_imageData = ippiMalloc_16s_AC4   ( _size.width, _size.height, &_stepBytes);
		ippiSet_16s_AC4R(values, _imageData, _size.width*2, _size);
	}
}

template<>
CIPPImage<Ipp16s>::CIPPImage(int width, int height, int nChannels, const Ipp16s* data )
{	
	_size.width = width;
	_size.height = height;
	_nChannels = nChannels;

	if(_nChannels == 1)
	{
		// memory allocation for image
		_imageData = ippiMalloc_16s_C1( _size.width, _size.height, &_stepBytes);

		// the pointer of our OpenCV data is our IPP image pointer
		ippiCopy_16s_C1R ( data, width*_nChannels*2 , _imageData, _size.width * _nChannels*2, _size );

		// function ippiMalloc allocates a memory block aligned to a 32-byte 
		// boundary for elements of different data types.
		// step size should be stepBytes
	}
	else if(_nChannels == 3)
	{
		_imageData = ippiMalloc_16s_C3( _size.width, _size.height, &_stepBytes);
		ippiCopy_16s_C3R ( data, width*_nChannels*2, _imageData, _size.width * _nChannels *2, _size );
	}
	else // if(img->_nChannels == 4)
	{
		_imageData = ippiMalloc_16s_AC4( _size.width, _size.height, &_stepBytes);
		ippiCopy_16s_AC4R( data, width*_nChannels*2, _imageData, _size.width * _nChannels*2, _size );
	}
}


/**
 * copy constructor
 **/
template<>
CIPPImage<Ipp16s>::CIPPImage(const CIPPImage<Ipp16s> &refImage)
{	
	assert(&refImage != NULL);
	assert(refImage.isInitialized());

	_nChannels = refImage._nChannels;
	_size = refImage._size;

	if(_nChannels == 1)
	{
		_imageData = ippiMalloc_16s_C1	( _size.width, _size.height, &_stepBytes);
		ippiCopy_16s_C1R ( refImage._imageData, _size.width * 2 *_nChannels, _imageData, _size.width * 2 * _nChannels, _size );
	}
	else if(_nChannels == 3)
	{
		_imageData = ippiMalloc_16s_C3	( _size.width, _size.height, &_stepBytes);
		ippiCopy_16s_C3R ( refImage._imageData, _size.width * 2 * _nChannels, _imageData, _size.width * 2 * _nChannels, _size );
	}
	else // if(img->nChannels == 4)
	{
		_imageData = ippiMalloc_16s_AC4   ( _size.width, _size.height, &_stepBytes);
		ippiCopy_16s_AC4R ( refImage._imageData, _size.width * 2 * _nChannels, _imageData, _size.width * 2 * _nChannels, _size );
	}
}


// ------------------------------------------------------
// Overloaded Operators 
// ------------------------------------------------------

// assignment operator
template<>
CIPPImage<Ipp16s>& CIPPImage<Ipp16s>::operator = (const CIPPImage<Ipp16s> &refImage)
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
		_imageData = ippiMalloc_16s_C1	( _size.width, _size.height, &_stepBytes);
		ippiCopy_16s_C1R ( refImage._imageData, _size.width * 2 *_nChannels, _imageData, _size.width * 2 * _nChannels, _size );
	}
	else if(_nChannels == 3)
	{
		_imageData = ippiMalloc_16s_C3	( _size.width, _size.height, &_stepBytes);
		ippiCopy_16s_C3R ( refImage._imageData, _size.width * 2 * _nChannels, _imageData, _size.width * 2 * _nChannels, _size );
	}
	else // if(img->nChannels == 4)
	{
		_imageData = ippiMalloc_16s_AC4   ( _size.width, _size.height, &_stepBytes);
		ippiCopy_16s_AC4R ( refImage._imageData, _size.width * 2 * _nChannels, _imageData, _size.width * 2 * _nChannels, _size );
	}

	return (*this);
}

// template<>	
// CIPPImage<Ipp8u>  operator+ (const CIPPImage<Ipp8u> &refImage) const;
template<>	
CIPPImage<Ipp16s> CIPPImage<Ipp16s>::operator+ (const CIPPImage<Ipp16s> &refImage) const
{
	CIPPImage<Ipp16s> result(_size, _nChannels);

	ippiAdd_16s_C1RSfs(refImage._imageData, refImage._size.width*2, _imageData, _size.width*2, 
						result._imageData, _size.width*2, _size, 0);  
	return result;
}


// notice that mask must be an Ipp8u image with value between 0 and 1
template<>
CIPPImage<Ipp16s> CIPPImage<Ipp16s>::operator* (const CIPPImage<Ipp16s> &mask)const
{
	CIPPImage<Ipp16s> result(_size, _nChannels);
	ippiMul_16s_C1RSfs(_imageData, _size.width*2, mask._imageData, mask._size.width*2, 
					result._imageData, result._size.width*2, mask._size, 0);
	return result;
}

template<>
CIPPImage<Ipp16s> CIPPImage<Ipp16s>::operator* (Ipp16s value)const
{
	CIPPImage<Ipp16s> result(_size, _nChannels);
	ippiMulC_16s_C1RSfs(_imageData, _size.width*2, value, 
		result._imageData, result._size.width*2, result._size, 0);

	return result;
}

template<>
CIPPImage<Ipp16s> CIPPImage<Ipp16s>::operator- (void) const
{
	return ((*this)*(-1));
}


// multiply the mask with the image at the center
// TODO: make sure the mask and the original image agrees with size,
// that is, no out of boundary multiplication
template<>
CIPPImage<Ipp16s> CIPPImage<Ipp16s>::multiplyCenter (const CIPPImage<Ipp16s> &mask, IppiPoint center)
{
	// currently assume, the mask located at the center of the image
	// is contained entirely inside of the image
	CIPPImage<Ipp16s> result(_size, _nChannels, 0);
	int xStart = int(center.x - ceil(double(mask._size.width)/2));
	int yStart = int(center.y - ceil(double(mask._size.width)/2));

	if(xStart >= 0 && yStart >= 0 && 
		xStart + mask._size.width <= _size.width && yStart+mask._size.height <=_size.height)
	{
		long offset = xStart + yStart * _size.width;
		ippiMul_16s_C1RSfs(_imageData + offset, _size.width*2, mask._imageData, mask._size.width*2,
			result._imageData+offset, result._size.width*2, mask._size, 0);
	}
	return result;
}


template<>
CIPPImage<Ipp16s> CIPPImage<Ipp16s>::operator/ (double value) const
{
	CIPPImage<Ipp16s> result(_size, _nChannels);

	ippiDivC_16s_C1RSfs(_imageData, _size.width*2, (Ipp16s)value, 
					result._imageData, result._size.width*2, result._size, 0);
	return result;
}

template<>
IppStatus CIPPImage<Ipp16s>::set(const Ipp16s value)
{
	if(NULL == _imageData)
		return ippStsNullPtrErr;

	if(_nChannels == 1)
	{
		return ippiSet_16s_C1R(value, _imageData, _size.width*2, _size);
	}
	else if(_nChannels == 3)
	{
		Ipp16s values[3] = {value, value, value};
		return ippiSet_16s_C3R(values, _imageData, _size.width*_nChannels*2, _size);
	}
	else // if(img->nChannels == 4)
	{
		Ipp16s values[4] = {value, value, value, value};
		return ippiSet_16s_AC4R(values, _imageData, _size.width*_nChannels*2, _size);
	}
}

template<>
IppStatus CIPPImage<Ipp16s>::set(const Ipp16s value, const CIPPImage<Ipp8u> &mask)
{
	if(NULL == _imageData)
		return ippStsNullPtrErr;

	assert(mask._size.height == _size.height && mask._size.width == _size.width);

	if(_nChannels != 1)
		return ippStsNullPtrErr;
	
	return ippiSet_16s_C1MR(value, _imageData, _size.width*2, _size, mask._imageData, _size.width);
}


template<>
IppStatus CIPPImage<Ipp16s>::setRegion(const Ipp16s value, IppiPoint start, IppiSize sizeROI)
{
	if(NULL == _imageData)
		return ippStsNullPtrErr;

	long offset = start.x + start.y * _size.width;

	if(_nChannels == 1)
	{	return ippiSet_16s_C1R(value, _imageData + offset, _size.width*2, sizeROI);
	}
	else if(_nChannels == 3)
	{	Ipp16s values[3] = {value, value, value};
		return ippiSet_16s_C3R(values, _imageData + offset, _size.width*2, sizeROI);
	}
	else // if(img->nChannels == 4)
	{	Ipp16s values[4] = {value, value, value, value};
		return ippiSet_16s_AC4R(values, _imageData + offset, _size.width*2, sizeROI);
	}
}


template<>
CIPPImage<Ipp16s> CIPPImage<Ipp16s>::getRegion(IppiPoint start, IppiSize sizeROI) const
{

	CIPPImage<Ipp16s> result(sizeROI, _nChannels, 0);
	long offset = (start.x + start.y * _size.width)*_nChannels;

	if(_nChannels == 1)
	{	ippiCopy_16s_C1R(_imageData + offset, _size.width*2, 
					result._imageData, result._size.width*_nChannels*2, sizeROI);
	}
	else if(_nChannels == 3)
	{	
		ippiCopy_16s_C3R(_imageData + offset, _size.width*2, 
					result._imageData, result._size.width*_nChannels*2, sizeROI);
	}
	else // if(img->nChannels == 4)
	{	
		ippiCopy_16s_AC4R(_imageData + offset, _size.width*2, 
					result._imageData, result._size.width*_nChannels*2, sizeROI);
	}
	return result;
}


template<>
IppStatus CIPPImage<Ipp16s>::copyRegion(IppiPoint start, const CIPPImage<Ipp16s> &patch, IppiPoint patchStart, IppiSize sizeROI)
{
	if(NULL == _imageData)
		return ippStsNullPtrErr;

	long offset = (start.x + start.y * _size.width)*_nChannels;
	long patchOffset = (patchStart.x + patchStart.y * patch._size.width)*_nChannels;

	if(_nChannels == 1)
	{	return ippiCopy_16s_C1R(patch._imageData+patchOffset, patch._size.width * _nChannels*2, _imageData + offset, _size.width * _nChannels*2, sizeROI);
	}
	else if(_nChannels == 3)
	{	
		return ippiCopy_16s_C3R(patch._imageData+patchOffset, patch._size.width * _nChannels*2, _imageData + offset, _size.width * _nChannels*2, sizeROI);
	}
	else // if(img->nChannels == 4)
	{	
		return ippiCopy_16s_AC4R(patch._imageData+patchOffset, patch._size.width * _nChannels*2, _imageData + offset, _size.width * _nChannels*2, sizeROI);
	}
}

// mask must be of the same size as the patch
template<>
IppStatus CIPPImage<Ipp16s>::copyRegionMask(IppiPoint start, const CIPPImage<Ipp16s> &patch, IppiPoint patchStart, 
										   IppiSize sizeROI, const CIPPImage<Ipp16s> &mask)
{
	CIPPImage<Ipp8u> mask8u = convert_16s8u(mask);
	if(NULL == _imageData)
		return ippStsNullPtrErr;

	long offset = (start.x + start.y * _size.width)*_nChannels;
	long patchOffset = (patchStart.x + patchStart.y * patch._size.width)*_nChannels;

	if(_nChannels == 1)
	{	
		return ippiCopy_16s_C1MR(patch._imageData+patchOffset, patch._size.width*_nChannels*2, 
			_imageData + offset, _size.width*_nChannels*2, sizeROI, mask8u._imageData, mask8u._size.width*_nChannels);
	}
	else if(_nChannels == 3)
	{	
		return ippiCopy_16s_C3MR(patch._imageData+patchOffset, patch._size.width*_nChannels*2, 
			_imageData + offset, _size.width*_nChannels*2, sizeROI, mask8u._imageData, mask8u._size.width*_nChannels);
	}
	else // if(img->nChannels == 4)
	{	
		return ippiCopy_16s_AC4MR(patch._imageData+patchOffset, patch._size.width*_nChannels*2, 
			_imageData + offset, _size.width*_nChannels*2, sizeROI, mask8u._imageData, mask8u._size.width*_nChannels);
	}
}

// mask must be of the same size as the patch
template<>
IppStatus CIPPImage<Ipp16s>::copyRegionMasked(IppiPoint start, const CIPPImage<Ipp16s> &patch, IppiPoint patchStart, 
										   IppiSize sizeROI, const CIPPImage<Ipp16s> &mask, IppiPoint maskStart)
{
	CIPPImage<Ipp8u> mask8u = convert_16s8u(mask);
	assert(1 == mask._nChannels);
	if(NULL == _imageData)
		return ippStsNullPtrErr;

	long offset = (start.x + start.y * _size.width)*_nChannels;
	long patchOffset = (patchStart.x + patchStart.y * patch._size.width)*_nChannels;
	long maskOffset = (maskStart.x + maskStart.y * mask._size.width);

	if(_nChannels == 1)
	{	
		return ippiCopy_16s_C1MR(patch._imageData+patchOffset, patch._size.width*_nChannels*2, 
			_imageData + offset, _size.width*_nChannels*2, sizeROI, mask8u._imageData+maskOffset, mask8u._size.width*_nChannels);
	}
	else if(_nChannels == 3)
	{	
		return ippiCopy_16s_C3MR(patch._imageData+patchOffset, patch._size.width*_nChannels*2, 
			_imageData + offset, _size.width*_nChannels*2, sizeROI, mask8u._imageData+maskOffset, mask8u._size.width*_nChannels);
	}
	else // if(img->nChannels == 4)
	{	
		return ippiCopy_16s_AC4MR(patch._imageData+patchOffset, patch._size.width*_nChannels*2, 
			_imageData + offset, _size.width*_nChannels*2, sizeROI, mask8u._imageData+maskOffset, mask8u._size.width*_nChannels);
	}
}

template<>
IppStatus CIPPImage<Ipp16s>::copyReplicateBorder(IppiPoint start, IppiSize sizeROI, 
												 const CIPPImage<Ipp16s> &src, IppiPoint srcStart, IppiSize sizeSrcROI, 
												 int topBorderWidth, int leftBorderWidth)
{
	if(NULL == _imageData)
		return ippStsNullPtrErr;

	long offset = start.x + start.y * _size.width;
	long srcOffset = srcStart.x + srcStart.y * src._size.width;

	if(_nChannels == 1)
	{	return ippiCopyReplicateBorder_16s_C1R(
			src._imageData+srcOffset, src._size.width * _nChannels*2, sizeSrcROI, 
			_imageData + offset, _size.width * _nChannels*2, sizeROI,
			topBorderWidth, leftBorderWidth);
	}
	else if(_nChannels == 3)
	{	
		return ippiCopyReplicateBorder_16s_C3R(
			src._imageData+srcOffset, src._size.width * _nChannels*2, sizeSrcROI, 
			_imageData + offset, _size.width * _nChannels*2, sizeROI,
			topBorderWidth, leftBorderWidth);
	}
	else // if(img->nChannels == 4)
	{	
		return ippiCopyReplicateBorder_16s_AC4R(
			src._imageData+srcOffset, src._size.width * _nChannels*2, sizeSrcROI, 
			_imageData + offset, _size.width * _nChannels*2, sizeROI,
			topBorderWidth, leftBorderWidth);
	}
}

template<>
double CIPPImage<Ipp16s>::sum() const
{
	Ipp64f result;
	ippiSum_16s_C1R(_imageData, _size.width*2, _size, &result);
	return double(result);
}

template<>
double CIPPImage<Ipp16s>::sum(IppiPoint start, IppiSize sizeROI) const
{
	Ipp64f result;
	int offset = start.x + start.y * _size.width;
	ippiSum_16s_C1R(_imageData + offset, _size.width*2, sizeROI, &result);
	return double(result);
}


template<>
Ipp16s CIPPImage<Ipp16s>::getMax(int *pIndexX, int *pIndexY) const
{
	Ipp16s value;

	ippiMaxIndx_16s_C1R(_imageData, _size.width*2, _size, &value, pIndexX, pIndexY); 
	return value;
}
	
template<>
Ipp16s CIPPImage<Ipp16s>::getMin(int *pIndexX, int *pIndexY) const
{
	Ipp16s value;

	ippiMinIndx_16s_C1R(_imageData, _size.width*2, _size, &value, pIndexX, pIndexY); 
	return value;
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
template<>
CIPPImage<Ipp16s> CIPPImage<Ipp16s>::getGradient(int direction) const
{
	IppStatus status;
	IppiSize sizeKernel;
	IppiSize sizeResult;
	IppiPoint anchor;

	//allocateImage(temp, _size, 1);
	//ippiConvert_8u16s_C1R(_imageData, _size.width, temp, _size.width * 2, _size);

	// The output value is computed as a sum of neighbor pixels¡¯ values, with kernel matrix
	// elements used as weight factors. Note that summation formulas implement a
	// convolution operation, which means that kernel coefficients are used in inverse order.
	Ipp32s kernel[2] = {1, -1};

	if(IPPIMAGE_HORIZONTAL == direction)
	{
		sizeResult.width = _size.width - 1;
		sizeResult.height = _size.height;
		sizeKernel.width = 2;
		sizeKernel.height = 1;
		anchor.x = 1;
		anchor.y = 0;
	}
	else
	{
		sizeResult.width = _size.width;
		sizeResult.height = _size.height - 1;
		sizeKernel.width = 1;
		sizeKernel.height = 2;
		anchor.x = 0;
		anchor.y = 1;
	}
	int divisor = 1;
 
	CIPPImage<Ipp16s> result(sizeResult, 1);

	status = ippiFilter_16s_C1R(_imageData, _size.width * 2, result._imageData, 
			sizeResult.width * 2, sizeResult, kernel, sizeKernel, anchor, divisor);
	// printIpp(pDst, *pSizeDst);

	if (status)
		displayStatus(status);
	return result;
}

template<>
CIPPImage<Ipp16s> CIPPImage<Ipp16s>::getGradientSameSize(int direction) const
{
	CIPPImage<Ipp16s> gradientValid;
	CIPPImage<Ipp16s> temp1(_size, _nChannels, Ipp16s(0));
	CIPPImage<Ipp16s> temp2(_size, _nChannels, Ipp16s(0));

	gradientValid = getGradient(direction);

	IppiPoint start1;
	IppiPoint start2;
	IppiPoint patchStart = {0, 0};
	IppiSize sizeROI = gradientValid._size;

	if(IPPIMAGE_HORIZONTAL == direction)
	{
		start1.x = 0;
		start1.y = 0;
		start2.x = 1;
		start2.y = 0;
	}
	else
	{
		start1.x = 0;
		start1.y = 0;
		start2.x = 0;
		start2.y = 1;
	}

	temp1.copyRegion(start1, gradientValid, patchStart, sizeROI);
	temp2.copyRegion(start2, gradientValid, patchStart, sizeROI);

	return ((temp1 + temp2) / 2); 
}

template<>
CIPPImage<Ipp16s> CIPPImage<Ipp16s>::getGradientMagnitude() const
{
	CIPPImage<Ipp16s> Dx = getGradientSameSize(IPPIMAGE_HORIZONTAL);
	CIPPImage<Ipp16s> Dy = getGradientSameSize(IPPIMAGE_VERTICAL);

	CIPPImage<Ipp16s> result(_size, _nChannels, Ipp16s(0));
	float dx, dy;

	for(int y = 0; y < _size.height; y++)
	{
		for(int x = 0; x < _size.width; x++)
		{
			dx = Dx.getPixel(x, y);
			dy = Dy.getPixel(x, y);
			result.setPixel(x, y, Ipp16s(sqrt(dx*dx + dy*dy))); 
		}
	}
	return result;
}



template<>
CIPPImage<Ipp16s> CIPPImage<Ipp16s>::getDivergence() const
{
	//CIPPImage<Ipp16s> Dx = getGradient(HORIZONTAL);
	//CIPPImage<Ipp16s> Dy = getGradient(VERTICAL);

	//CIPPImage<Ipp16s> result;
	//result = ::getDivergence(Dx, Dy);
	return ::getDivergence(getGradient(IPPIMAGE_HORIZONTAL), getGradient(IPPIMAGE_VERTICAL));
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
CIPPImage<Ipp16s> CIPPImage<Ipp16s>::rotateAroundCenter(double angle, double * pXShift, double * pYShift, int interpolation) const
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

	CIPPImage<Ipp16s> result(sizeResult, _nChannels, 0);

	switch(_nChannels)
	{
		// if only one channel
	case 1:
		// ippiSet_16u_C1R(0, result._imageData, sizeResult.width, sizeResult); 

		status = ippiRotate_16u_C1R((Ipp16u*)_imageData, _size, _size.width * 2, rectS, 
			(Ipp16u*)result._imageData, sizeResult.width * 2, rectR, angle, 
			xShift - bound[0][0], yShift - bound[0][1], interpolation);
		// printf( " %d : %s\n", status, ippCoreGetStatusString( status));
		break;
	}


	// TODO:DEBUG
	//// another possibility is to convert it to 32f first
	//CIPPImage<Ipp32f> temp = convertUp(*this);
	//CIPPImage<Ipp32f> resultTemp = convertUp(result);
	//switch(_nChannels)
	//{
	//	// if only one channel
	//case 1:
	//	status = ippiRotate_32f_C1R(temp._imageData, _size, _size.width*4, rectS, 
	//		resultTemp._imageData, sizeResult.width * 2, rectR, angle, 
	//		xShift - bound[0][0], yShift - bound[0][1], interpolation);
	//	result = converDown(resultTemp);
	//	break;
	//}

	* pXShift = xShift;
	* pYShift = yShift;

	if (status)
		displayStatus(status);
	return result;
}

template<>
CIPPImage<Ipp16s> CIPPImage<Ipp16s>::rotateCenter(double angle, double xCenter, double yCenter, int interpolation) const
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

	CIPPImage<Ipp16s> result(sizeResult, _nChannels, 0);

	switch(_nChannels)
	{
		// if only one channel
	case 1:
		// ippiSet_16u_C1R(0, result._imageData, sizeResult.width, sizeResult); 

		status = ippiRotate_16u_C1R((Ipp16u*)_imageData, _size, _size.width * 2, rectS, 
			(Ipp16u*)result._imageData, sizeResult.width * 2, rectR, angle, 
			xShift - bound[0][0], yShift - bound[0][1], interpolation);
		// printf( " %d : %s\n", status, ippCoreGetStatusString( status));
		break;
	}


	// TODO:DEBUG
	//// another possibility is to convert it to 32f first
	//CIPPImage<Ipp32f> temp = convertUp(*this);
	//CIPPImage<Ipp32f> resultTemp = convertUp(result);
	//switch(_nChannels)
	//{
	//	// if only one channel
	//case 1:
	//	status = ippiRotate_32f_C1R(temp._imageData, _size, _size.width*4, rectS, 
	//		resultTemp._imageData, sizeResult.width * 2, rectR, angle, 
	//		xShift - bound[0][0], yShift - bound[0][1], interpolation);
	//	result = converDown(resultTemp);
	//	break;
	//}


	if (status)
		displayStatus(status);
	return result;
}



template<>
CIPPImage<Ipp16s> CIPPImage<Ipp16s>::getRotatedSquarePatch(IppiPoint rotationAxis, double angleInDegree, int radius) const
{

	// the large radius needed for the rotation
	//int radiusLarge = (int)ceil(sqrt(double(2.0))*radius);
	int radiusLarge = int(ceil(radius * 1.414213562));

	IppiSize sizePatch = {2*radius + 1, 2*radius + 1}; 
	IppiSize sizePatchLarge = {2*radiusLarge+1, 2*radiusLarge+1};

	// CIPPImage<Ipp16s> result(sizePatch, _nChannels, 0);
	// not initialized if failure
	CIPPImage<Ipp16s> result;

	IppiPoint start;
	start.x = rotationAxis.x - radiusLarge;
	start.y = rotationAxis.y - radiusLarge;

	if(start.x >= 0 && start.y >= 0 && 
		start.x + sizePatchLarge.width <= _size.width && 
		start.y + sizePatchLarge.height <= _size.height)
	{
		CIPPImage<Ipp16s> patchLarge = getRegion(start, sizePatchLarge);

		double xShift, yShift;
		CIPPImage<Ipp16s> patchLargeRotated = patchLarge.rotateAroundCenter(angleInDegree, 
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
CIPPImage<Ipp16s> CIPPImage<Ipp16s>::mirror(IppiAxis flip) const
{
	CIPPImage<Ipp16s> result(_size, _nChannels);
	switch(_nChannels)
	{
	case 1:
		ippiMirror_16u_C1R((Ipp16u*)_imageData, _size.width*2, (Ipp16u*)result._imageData, _size.width*2, _size, flip);
		break;
	case 3:
		ippiMirror_16u_C3R((Ipp16u*)_imageData, _size.width*_nChannels*2, (Ipp16u*)result._imageData, _size.width*_nChannels*2, _size, flip);
		break;
	case 4:
		ippiMirror_16u_AC4R((Ipp16u*)_imageData, _size.width*_nChannels*2, (Ipp16u*)result._imageData, _size.width*_nChannels*2, _size, flip);
		break;
	}
	return result;

	// TODO:DEBUG
	//// another possibility is to convert it to 32f first
	//CIPPImage<Ipp32f> temp = convertUp(*this);
	//CIPPImage<Ipp32f> resultTemp = convertUp(result);
	//switch(_nChannels)
	//{
	//	// if only one channel
	//case 1:
	//	status = ippiRotate_32f_C1R(temp._imageData, _size, _size.width*4, rectS, 
	//		resultTemp._imageData, sizeResult.width * 2, rectR, angle, 
	//		xShift - bound[0][0], yShift - bound[0][1], interpolation);
	//	result = converDown(resultTemp);
	//	break;
	//}
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
int CIPPImage<Ipp16s>::countInRange(IppiPoint start, IppiSize sizeRegion, Ipp16s lowerBound, Ipp16s upperBound) const
{
	int counts;
	CIPPImage<Ipp32f> this32f = convert_16s32f(*this);

	int offset = start.x + start.y * _size.width;
	ippiCountInRange_32f_C1R(this32f._imageData + offset, _size.width*4, sizeRegion, &counts, (Ipp32f)lowerBound, (Ipp32f)upperBound);

	return counts;
}

template<>
int CIPPImage<Ipp16s>::countInRange(Ipp16s lowerBound, Ipp16s upperBound) const
{
	int counts;
	CIPPImage<Ipp32f> this32f = convert_16s32f(*this);

	ippiCountInRange_32f_C1R(this32f._imageData, _size.width*4, _size, &counts, (Ipp32f)lowerBound, (Ipp32f)upperBound);

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
CIPPImage<Ipp16s> remap(const CIPPImage<Ipp16s> &source, 
					   const CIPPImage<Ipp32f> &xMap, const CIPPImage<Ipp32f> &yMap,
					   IppiSize resultSize, int interpolation)
{
	IppStatus status;

	IppiSize size = source._size;
	CIPPImage<Ipp32f> sourceTemp = convert_16s32f(source);
	CIPPImage<Ipp32f> resultTemp(resultSize, source._nChannels, 0);

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
	status = ippiRemap_32f_C1R(sourceTemp._imageData, size, size.width*4, ROI,
		xMap._imageData, resultSize.width*4,
		yMap._imageData, resultSize.width*4,
		resultTemp._imageData, resultSize.width*4, resultSize, IPPI_INTER_CUBIC);

	CIPPImage<Ipp16s> result = convert_32f16s(resultTemp);

	if (status)
		displayStatus(status);
	return result;
}

template<>
CIPPImage<Ipp16s> CIPPImage<Ipp16s>::remap(const CIPPImage<Ipp32f> &xMap, const CIPPImage<Ipp32f> &yMap,
					   IppiSize resultSize, int interpolation) const
{
	IppStatus status;

	CIPPImage<Ipp32f> sourceTemp = convert_16s32f(*this);
	CIPPImage<Ipp32f> resultTemp(resultSize, _nChannels, 0);

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
	status = ippiRemap_32f_C1R(sourceTemp._imageData, _size, _size.width*4, ROI,
		xMap._imageData, resultSize.width*4,
		yMap._imageData, resultSize.width*4,
		resultTemp._imageData, resultSize.width*4, resultSize, IPPI_INTER_CUBIC);

	CIPPImage<Ipp16s> result = convert_32f16s(resultTemp);

	if (status)
		displayStatus(status);
	return result;
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
CIPPImage<Ipp16s> alphaComp(const CIPPImage<Ipp16s> &A, const CIPPImage<Ipp16s> &B, IppiAlphaType alphaType)
{
	assert(A._size.width == B._size.width && A._size.height == B._size.height &&
		A._nChannels == B._nChannels);

	IppiSize size = A._size;
	int nChannels = A._nChannels;

	CIPPImage<Ipp16s> result(size, nChannels);

	ippiAlphaComp_16u_AC1R((const Ipp16u*)A._imageData, size.width*2,
		(const Ipp16u*)B._imageData, size.width*2,
		(Ipp16u*)result._imageData, size.width*2, size,
		alphaType);
	
	return result;
}


template<>
inline CIPPImage<Ipp16s> CIPPImage<Ipp16s>::RGBToGray() const
{
	if(1 == _nChannels) return (*this);

	CIPPImage<Ipp16s> result(_size, 1);
	if(3 == _nChannels)
		ippiRGBToGray_16s_C3C1R(_imageData, 
			_size.width*_nChannels*2,
			result._imageData,
			result._size.width*2,
			_size);
	else // if(4 == _nChannels)
		ippiRGBToGray_16s_AC4C1R(_imageData, 
			_size.width*_nChannels*2,
			result._imageData,
			result._size.width*2,
			_size);

	return result;
}
