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
 * @file IPPImage_16u.cpp
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
CIPPImage<Ipp16u>::CIPPImage(IppiSize size, int nChannels)
{
	if(nChannels == 1)
		_imageData = ippiMalloc_16u_C1	( size.width, size.height, &_stepBytes);
	else if(nChannels == 3)
		_imageData = ippiMalloc_16u_C3	( size.width, size.height, &_stepBytes);
	else // if(img->nChannels == 4)
		_imageData = ippiMalloc_16u_AC4   ( size.width, size.height, &_stepBytes);

	_size = size;
	_nChannels = nChannels;
}

template<>
CIPPImage<Ipp16u>::CIPPImage(int width, int height, int nChannels )
{

	if(nChannels == 1)
		_imageData = ippiMalloc_16u_C1	( width, height, &_stepBytes);
	else if(nChannels == 3)
		_imageData = ippiMalloc_16u_C3	( width, height, &_stepBytes);
	else // if(img->nChannels == 4)
		_imageData = ippiMalloc_16u_AC4   ( width, height, &_stepBytes);

	_size.width = width;
	_size.height = height;
	_nChannels = nChannels;
}

template<>
CIPPImage<Ipp16u>::CIPPImage(IppiSize size, int nChannels, const Ipp16u value)
{

	_size = size;
	_nChannels = nChannels;

	Ipp16s value16s = value;

	if(_nChannels == 1)
	{
		_imageData = ippiMalloc_16u_C1	( _size.width, _size.height, &_stepBytes);
		ippiSet_16s_C1R(value16s, (Ipp16s*)_imageData, _size.width*2, _size);
	}
	else if(_nChannels == 3)
	{
		Ipp16s values16s[3] = {value16s, value16s, value16s};
		_imageData = ippiMalloc_16u_C3	( _size.width, _size.height, &_stepBytes);
		ippiSet_16s_C3R(values16s, (Ipp16s*)_imageData, _size.width*2, _size);
	}
	else // if(img->nChannels == 4)
	{
		Ipp16s values16s[4] = {value16s, value16s, value16s, value16s};
		_imageData = ippiMalloc_16u_AC4   ( _size.width, _size.height, &_stepBytes);
		ippiSet_16s_AC4R(values16s, (Ipp16s*)_imageData, _size.width*2, _size);
	}
}


template<>
CIPPImage<Ipp16u>::CIPPImage(int width, int height, int nChannels, const Ipp16u value )
{
	_size.width = width;
	_size.height = height;
	_nChannels = nChannels;

	Ipp16s value16s = value;

	if(_nChannels == 1)
	{
		_imageData = ippiMalloc_16u_C1	( _size.width, _size.height, &_stepBytes);
		ippiSet_16s_C1R(value16s, (Ipp16s*)_imageData, _size.width*2, _size);
	}
	else if(_nChannels == 3)
	{
		Ipp16s values16s[3] = {value16s, value16s, value16s};
		_imageData = ippiMalloc_16u_C3	( _size.width, _size.height, &_stepBytes);
		ippiSet_16s_C3R(values16s, (Ipp16s*)_imageData, _size.width*2, _size);
	}
	else // if(img->nChannels == 4)
	{
		Ipp16s values16s[4] = {value16s, value16s, value16s, value16s};
		_imageData = ippiMalloc_16u_AC4   ( _size.width, _size.height, &_stepBytes);
		ippiSet_16s_AC4R(values16s, (Ipp16s*)_imageData, _size.width*2, _size);
	}
}



template<>
CIPPImage<Ipp16u>::CIPPImage(int width, int height, int nChannels, const Ipp16u* data )
{	
	_size.width = width;
	_size.height = height;
	_nChannels = nChannels;

	if(_nChannels == 1)
	{
		// memory allocation for image
		_imageData = ippiMalloc_16u_C1( _size.width, _size.height, &_stepBytes);

		// the pointer of our OpenCV data is our IPP image pointer
		ippiCopy_16s_C1R ( (Ipp16s*)data, width*_nChannels*2 , (Ipp16s*)_imageData, _size.width * _nChannels*2, _size );

		// function ippiMalloc allocates a memory block aligned to a 32-byte 
		// boundary for elements of different data types.
		// step size should be stepBytes
	}
	else if(_nChannels == 3)
	{
		_imageData = ippiMalloc_16u_C3( _size.width, _size.height, &_stepBytes);
		ippiCopy_16s_C3R ( (Ipp16s*)data, width*_nChannels*2, (Ipp16s*)_imageData, _size.width * _nChannels *2, _size );
	}
	else // if(img->_nChannels == 4)
	{
		_imageData = ippiMalloc_16u_AC4( _size.width, _size.height, &_stepBytes);
		ippiCopy_16s_AC4R( (Ipp16s*)data, width*_nChannels*2, (Ipp16s*)_imageData, _size.width * _nChannels*2, _size );
	}
}


/**
 * copy constructor
 **/
template<>
CIPPImage<Ipp16u>::CIPPImage(const CIPPImage<Ipp16u> &refImage)
{
	assert(&refImage != NULL);
	assert(refImage.isInitialized());

	_nChannels = refImage._nChannels;
	_size = refImage._size;

	if(_nChannels == 1)
	{
		_imageData = ippiMalloc_16u_C1	( _size.width, _size.height, &_stepBytes);
		ippiCopy_16s_C1R ( (Ipp16s*)refImage._imageData, _size.width * 2 *_nChannels, 
			(Ipp16s*)_imageData, _size.width * 2 * _nChannels, _size );
	}
	else if(_nChannels == 3)
	{
		_imageData = ippiMalloc_16u_C3	( _size.width, _size.height, &_stepBytes);
		ippiCopy_16s_C3R ( (Ipp16s*)refImage._imageData, _size.width * 2 * _nChannels, 
			(Ipp16s*)_imageData, _size.width * 2 * _nChannels, _size );
	}
	else // if(img->nChannels == 4)
	{
		_imageData = ippiMalloc_16u_AC4   ( _size.width, _size.height, &_stepBytes);
		ippiCopy_16s_AC4R ( (Ipp16s*)refImage._imageData, _size.width * 2 * _nChannels, 
			(Ipp16s*)_imageData, _size.width * 2 * _nChannels, _size );
	}
}


// ------------------------------------------------------
// Overloaded Operators 
// ------------------------------------------------------

// assignment operator
template<>
CIPPImage<Ipp16u>& CIPPImage<Ipp16u>::operator = (const CIPPImage<Ipp16u> &refImage)
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
		_imageData = ippiMalloc_16u_C1	( _size.width, _size.height, &_stepBytes);
		ippiCopy_16s_C1R ( (Ipp16s*)refImage._imageData, _size.width * 2 *_nChannels, 
			(Ipp16s*)_imageData, _size.width * 2 * _nChannels, _size );
	}
	else if(_nChannels == 3)
	{
		_imageData = ippiMalloc_16u_C3	( _size.width, _size.height, &_stepBytes);
		ippiCopy_16s_C3R ( (Ipp16s*)refImage._imageData, _size.width * 2 * _nChannels, 
			(Ipp16s*)_imageData, _size.width * 2 * _nChannels, _size );
	}
	else // if(img->nChannels == 4)
	{
		_imageData = ippiMalloc_16u_AC4   ( _size.width, _size.height, &_stepBytes);
		ippiCopy_16s_AC4R ( (Ipp16s*)refImage._imageData, _size.width * 2 * _nChannels, 
			(Ipp16s*)_imageData, _size.width * 2 * _nChannels, _size );
	}

	return (*this);
}

// template<>	
// CIPPImage<Ipp8u>  operator+ (const CIPPImage<Ipp8u> &refImage) const;
template<>	
CIPPImage<Ipp16u> CIPPImage<Ipp16u>::operator+ (const CIPPImage<Ipp16u> &refImage) const
{
	CIPPImage<Ipp32f> result32f(_size, _nChannels);
	CIPPImage<Ipp32f> refImage32f = convert_16u32f(refImage);
	CIPPImage<Ipp32f> this32f = convert_16u32f(*this);
	
	ippiAdd_32f_C1R(refImage32f._imageData, refImage32f._size.width*4, 
					this32f._imageData, _size.width*4, 
					result32f._imageData, _size.width*4, _size);  

	return convert_32f16u(result32f);
}


template<> 
CIPPImage<Ipp16u> operator+ (const CIPPImage<Ipp16u> &image, Ipp16u value)
{
	CIPPImage<Ipp32f> result(image._size, 1, 0);
	CIPPImage<Ipp32f> image32f = convert_16u32f(image);

	ippiAddC_32f_C1R(image32f._imageData, image32f._size.width*4,
		value, result._imageData, result._size.width*4, image32f._size);

	return convert_32f16u(result);
}


template<> 
CIPPImage<Ipp16u> operator+ (Ipp16u value, const CIPPImage<Ipp16u> &image)
{
	CIPPImage<Ipp32f> result(image._size, 1, 0);
	CIPPImage<Ipp32f> image32f = convert_16u32f(image);

	ippiAddC_32f_C1R(image32f._imageData, image32f._size.width*4,
		value, result._imageData, result._size.width*4, image32f._size);

	return convert_32f16u(result);
}


// notice that mask must be an Ipp8u image with value between 0 and 1
template<>
CIPPImage<Ipp16u> CIPPImage<Ipp16u>::operator* (const CIPPImage<Ipp16u> &mask)const
{
	CIPPImage<Ipp32f> result32f(_size, _nChannels);
	CIPPImage<Ipp32f> mask32f = convert_16u32f(mask);
	CIPPImage<Ipp32f> this32f = convert_16u32f(*this);

	ippiMul_32f_C1R(this32f._imageData, _size.width*4, 
		mask32f._imageData, mask32f._size.width*4, 
		result32f._imageData, result32f._size.width*4, mask32f._size);
	return convert_32f16u(result32f);
}

template<>
CIPPImage<Ipp16u> CIPPImage<Ipp16u>::operator* (Ipp16u value)const
{
	CIPPImage<Ipp32f> result32f(_size, _nChannels);
	CIPPImage<Ipp32f> this32f = convert_16u32f(*this);
	
	ippiMulC_32f_C1R(this32f._imageData, _size.width*4, (Ipp32f)value, 
		result32f._imageData, result32f._size.width*4, result32f._size);

	return convert_32f16u(result32f);
}

template<>
CIPPImage<Ipp16u> CIPPImage<Ipp16u>::operator- (void) const
{
	return ((*this)*(-1));
}

// multiply the mask with the image at the center
// TODO: make sure the mask and the original image agrees with size,
// that is, no out of boundary multiplication
template<>
CIPPImage<Ipp16u> CIPPImage<Ipp16u>::multiplyCenter (const CIPPImage<Ipp16u> &mask, IppiPoint center)
{
	// currently assume, the mask located at the center of the image
	// is contained entirely inside of the image
	CIPPImage<Ipp32f> result32f(_size, _nChannels, 0);
	CIPPImage<Ipp32f> mask32f = convert_16u32f(mask);
	CIPPImage<Ipp32f> this32f = convert_16u32f(*this);

	int xStart = int(center.x - ceil(double(mask._size.width)/2));
	int yStart = int(center.y - ceil(double(mask._size.width)/2));

	if(xStart >= 0 && yStart >= 0 && 
		xStart + mask._size.width <= _size.width && yStart+mask._size.height <=_size.height)
	{
		long offset = xStart + yStart * _size.width;
		ippiMul_32f_C1R(this32f._imageData + offset, _size.width*4, 
			mask32f._imageData, mask32f._size.width*4,
			result32f._imageData+offset, result32f._size.width*4, mask32f._size);
	}
	return convert_32f16u(result32f);
}


template<>
CIPPImage<Ipp16u> CIPPImage<Ipp16u>::operator/ (double value) const
{
	CIPPImage<Ipp32f> result32f(_size, _nChannels);
	CIPPImage<Ipp32f> this32f = convert_16u32f(*this);

	ippiDivC_32f_C1R(this32f._imageData, _size.width*4, (Ipp32f)value, 
					result32f._imageData, result32f._size.width*4, result32f._size);
	return convert_32f16u(result32f);
}

template<>
IppStatus CIPPImage<Ipp16u>::set(const Ipp16u value)
{
	if(NULL == _imageData)
		return ippStsNullPtrErr;

	Ipp16s value16s = value;

	if(_nChannels == 1)
	{
		return ippiSet_16s_C1R(value16s, (Ipp16s*)_imageData, _size.width*2, _size);
	}
	else if(_nChannels == 3)
	{
		Ipp16s values16s[3] = {value16s, value16s, value16s};
		return ippiSet_16s_C3R(values16s, (Ipp16s*)_imageData, _size.width*_nChannels*2, _size);
	}
	else // if(img->nChannels == 4)
	{
		Ipp16s values16s[4] = {value16s, value16s, value16s, value16s};
		return ippiSet_16s_AC4R(values16s, (Ipp16s*)_imageData, _size.width*_nChannels*2, _size);
	}
}


template<>
IppStatus CIPPImage<Ipp16u>::set(const Ipp16u value, const CIPPImage<Ipp8u> &mask)
{
	if(NULL == _imageData)
		return ippStsNullPtrErr;

	assert(mask._size.height == _size.height && mask._size.width == _size.width);

	Ipp16s value16s = value;

	if(_nChannels != 1)
		return ippStsNullPtrErr;

	return ippiSet_16s_C1MR(value16s, (Ipp16s*)_imageData, _size.width*2, _size, mask._imageData, _size.width);
}


template<>
IppStatus CIPPImage<Ipp16u>::setRegion(const Ipp16u value, IppiPoint start, IppiSize sizeROI)
{
	if(NULL == _imageData)
		return ippStsNullPtrErr;

	long offset = start.x + start.y * _size.width;
	
	Ipp16s value16s = value;

	if(_nChannels == 1)
	{	return ippiSet_16s_C1R(value16s, (Ipp16s*)_imageData + offset, _size.width*2, sizeROI);
	}
	else if(_nChannels == 3)
	{	Ipp16s values16s[3] = {value16s, value16s, value16s};
		return ippiSet_16s_C3R(values16s, (Ipp16s*)_imageData + offset, _size.width*2, sizeROI);
	}
	else // if(img->nChannels == 4)
	{	Ipp16s values16s[4] = {value16s, value16s, value16s, value16s};
		return ippiSet_16s_AC4R(values16s, (Ipp16s*)_imageData + offset, _size.width*2, sizeROI);
	}
}

template<>
CIPPImage<Ipp16u> CIPPImage<Ipp16u>::getRegion(IppiPoint start, IppiSize sizeROI) const
{

	CIPPImage<Ipp16u> result(sizeROI, _nChannels, 0);
	long offset = start.x + start.y * _size.width;

	if(_nChannels == 1)
	{	ippiCopy_16s_C1R((Ipp16s*)_imageData + offset, _size.width*2, 
					(Ipp16s*)result._imageData, result._size.width*2, sizeROI);
	}
	else if(_nChannels == 3)
	{	
		ippiCopy_16s_C3R((Ipp16s*)_imageData + offset, _size.width*2, 
					(Ipp16s*)result._imageData, result._size.width*2, sizeROI);
	}
	else // if(img->nChannels == 4)
	{	
		ippiCopy_16s_AC4R((Ipp16s*)_imageData + offset, _size.width*2, 
					(Ipp16s*)result._imageData, result._size.width*2, sizeROI);
	}

	return result;
}

template<>
IppStatus CIPPImage<Ipp16u>::copyRegion(IppiPoint start, const CIPPImage<Ipp16u> &patch, IppiPoint patchStart, IppiSize sizeROI)
{
	if(NULL == _imageData)
		return ippStsNullPtrErr;

	long offset = (start.x + start.y * _size.width)*_nChannels;
	long patchOffset = (patchStart.x + patchStart.y * patch._size.width)*_nChannels;

	if(_nChannels == 1)
	{	return ippiCopy_16s_C1R((Ipp16s*)patch._imageData+patchOffset, patch._size.width * _nChannels*2, 
			(Ipp16s*)_imageData + offset, _size.width * _nChannels*2, sizeROI);
	}
	else if(_nChannels == 3)
	{	
		return ippiCopy_16s_C3R((Ipp16s*)patch._imageData+patchOffset, patch._size.width * _nChannels*2, 
			(Ipp16s*)_imageData + offset, _size.width * _nChannels*2, sizeROI);
	}
	else // if(img->nChannels == 4)
	{	
		return ippiCopy_16s_AC4R((Ipp16s*)patch._imageData+patchOffset, patch._size.width * _nChannels*2, 
			(Ipp16s*)_imageData + offset, _size.width * _nChannels*2, sizeROI);
	}
}

// mask must be of the same size as the patch
template<>
IppStatus CIPPImage<Ipp16u>::copyRegionMask(IppiPoint start, const CIPPImage<Ipp16u> &patch, IppiPoint patchStart, 
										   IppiSize sizeROI, const CIPPImage<Ipp16u> &mask)
{
	CIPPImage<Ipp8u> mask8u = convert_16u8u(mask);
	if(NULL == _imageData)
		return ippStsNullPtrErr;

	long offset = (start.x + start.y * _size.width)*_nChannels;
	long patchOffset = (patchStart.x + patchStart.y * patch._size.width)*_nChannels;

	if(_nChannels == 1)
	{	
		return ippiCopy_16s_C1MR((Ipp16s*)patch._imageData+patchOffset, patch._size.width*_nChannels*2, 
			(Ipp16s*)_imageData + offset, _size.width*_nChannels*2, sizeROI, 
			mask8u._imageData, mask8u._size.width*_nChannels);
	}
	else if(_nChannels == 3)
	{	
		return ippiCopy_16s_C3MR((Ipp16s*)patch._imageData+patchOffset, patch._size.width*_nChannels*2, 
			(Ipp16s*)_imageData + offset, _size.width*_nChannels*2, sizeROI, 
			mask8u._imageData, mask8u._size.width*_nChannels);
	}
	else // if(img->nChannels == 4)
	{	
		return ippiCopy_16s_AC4MR((Ipp16s*)patch._imageData+patchOffset, patch._size.width*_nChannels*2, 
			(Ipp16s*)_imageData + offset, _size.width*_nChannels*2, sizeROI, 
			mask8u._imageData, mask8u._size.width*_nChannels);
	}
}


// mask must be of the same size as the patch
template<>
IppStatus CIPPImage<Ipp16u>::copyRegionMasked(IppiPoint start, const CIPPImage<Ipp16u> &patch, IppiPoint patchStart, 
										   IppiSize sizeROI, const CIPPImage<Ipp16u> &mask, IppiPoint maskStart)
{
	CIPPImage<Ipp8u> mask8u = convert_16u8u(mask);
	assert(1 == mask._nChannels);
	if(NULL == _imageData)
		return ippStsNullPtrErr;

	long offset = (start.x + start.y * _size.width)*_nChannels;
	long patchOffset = (patchStart.x + patchStart.y * patch._size.width)*_nChannels;
	long maskOffset = (maskStart.x + maskStart.y * mask._size.width);

	if(_nChannels == 1)
	{	
		return ippiCopy_16s_C1MR((Ipp16s*)patch._imageData+patchOffset, patch._size.width*_nChannels*2, 
			(Ipp16s*)_imageData + offset, _size.width*_nChannels*2, sizeROI, 
			mask8u._imageData+maskOffset, mask8u._size.width*_nChannels);
	}
	else if(_nChannels == 3)
	{	
		return ippiCopy_16s_C3MR((Ipp16s*)patch._imageData+patchOffset, patch._size.width*_nChannels*2, 
			(Ipp16s*)_imageData + offset, _size.width*_nChannels*2, sizeROI, 
			mask8u._imageData+maskOffset, mask8u._size.width*_nChannels);
	}
	else // if(img->nChannels == 4)
	{	
		return ippiCopy_16s_AC4MR((Ipp16s*)patch._imageData+patchOffset, patch._size.width*_nChannels*2, 
			(Ipp16s*)_imageData + offset, _size.width*_nChannels*2, sizeROI, 
			mask8u._imageData+maskOffset, mask8u._size.width*_nChannels);
	}
}

template<>
IppStatus CIPPImage<Ipp16u>::copyConstBorder(IppiPoint start, IppiSize sizeROI, 
												 const CIPPImage<Ipp16u> &src, IppiPoint srcStart, IppiSize sizeSrcROI, 
												 int topBorderWidth, int leftBorderWidth, Ipp16u value)
{
	if(NULL == _imageData)
		return ippStsNullPtrErr;

	long offset = start.x + start.y * _size.width;
	long srcOffset = srcStart.x + srcStart.y * src._size.width;

	if(_nChannels == 1)
	{	return ippiCopyConstBorder_16s_C1R(
			(Ipp16s*)src._imageData+srcOffset, src._size.width * _nChannels*2, sizeSrcROI, 
			(Ipp16s*)_imageData + offset, _size.width * _nChannels*2, sizeROI,
			topBorderWidth, leftBorderWidth, value);
	}
	else if(_nChannels == 3)
	{	
		return ippStsNullPtrErr;
	}
	else // if(img->nChannels == 4)
	{	
		return ippStsNullPtrErr;
	}
}


template<>
IppStatus CIPPImage<Ipp16u>::copyReplicateBorder(IppiPoint start, IppiSize sizeROI, 
												 const CIPPImage<Ipp16u> &src, IppiPoint srcStart, IppiSize sizeSrcROI, 
												 int topBorderWidth, int leftBorderWidth)
{
	if(NULL == _imageData)
		return ippStsNullPtrErr;

	long offset = start.x + start.y * _size.width;
	long srcOffset = srcStart.x + srcStart.y * src._size.width;

	if(_nChannels == 1)
	{	return ippiCopyReplicateBorder_16s_C1R(
			(Ipp16s*)src._imageData+srcOffset, src._size.width * _nChannels*2, sizeSrcROI, 
			(Ipp16s*)_imageData + offset, _size.width * _nChannels*2, sizeROI,
			topBorderWidth, leftBorderWidth);
	}
	else if(_nChannels == 3)
	{	
		return ippiCopyReplicateBorder_16s_C3R(
			(Ipp16s*)src._imageData+srcOffset, src._size.width * _nChannels*2, sizeSrcROI, 
			(Ipp16s*)_imageData + offset, _size.width * _nChannels*2, sizeROI,
			topBorderWidth, leftBorderWidth);
	}
	else // if(img->nChannels == 4)
	{	
		return ippiCopyReplicateBorder_16s_AC4R(
			(Ipp16s*)src._imageData+srcOffset, src._size.width * _nChannels*2, sizeSrcROI, 
			(Ipp16s*)_imageData + offset, _size.width * _nChannels*2, sizeROI,
			topBorderWidth, leftBorderWidth);
	}
}

template<>
double CIPPImage<Ipp16u>::sum() const
{
	CIPPImage<Ipp32f> source32f = convert_16u32f(*this);
	Ipp64f result;
	ippiSum_32f_C1R(source32f._imageData, _size.width*4, _size, &result, ippAlgHintNone);
	return double(result);
}

template<>
double CIPPImage<Ipp16u>::sum(IppiPoint start, IppiSize sizeROI) const
{
	CIPPImage<Ipp32f> source32f = convert_16u32f(*this);

	Ipp64f result;
	int offset = start.x + start.y * _size.width;
	ippiSum_32f_C1R(source32f._imageData + offset, _size.width*4, sizeROI, &result, ippAlgHintNone);
	return double(result);
}


template<>
Ipp16u CIPPImage<Ipp16u>::getMax(int *pIndexX, int *pIndexY) const
{
	Ipp32f value32f;
	CIPPImage<Ipp32f> this32f = convert_16u32f(*this);

	ippiMaxIndx_32f_C1R(this32f._imageData, _size.width*4, _size, &value32f, pIndexX, pIndexY);
	return (Ipp16u)value32f;
}
	
template<>
Ipp16u CIPPImage<Ipp16u>::getMin(int *pIndexX, int *pIndexY) const
{
	Ipp32f value32f;
	CIPPImage<Ipp32f> this32f = convert_16u32f(*this);

	ippiMinIndx_32f_C1R(this32f._imageData, _size.width*4, _size, &value32f, pIndexX, pIndexY);
	return (Ipp16u)value32f;
}


/**
 * IppStatus getGradient(const Ipp8u* pSrc, IppiSize sizeSrc, Ipp16u* &pDst, IppiSize * pSizeDst, int nChannels, int direction)
 * get the gradient of the source image
 * @param pSrc pointer to the source image
 * @param sizeSrc size of the source image
 * @param pDst reference pointer to the destination image
 * @param pSizeDst pointer to the size of the destination image
 * @param nChannels number of channels
 * @param direction horizontal or vertical direction)
 * @return IppStatus status of the filtering operation
 **/
CIPPImage<Ipp32f> getGradient(const CIPPImage<Ipp16u> &image, int direction)
{
	CIPPImage<Ipp32f> image32f = convert_16u32f(image);
	return image32f.getGradient(direction);	
}

// blured effects will appear
CIPPImage<Ipp32f> getGradientSameSize(const CIPPImage<Ipp16u> &image,int direction)
{
	CIPPImage<Ipp32f> image32f = convert_16u32f(image);
	return image32f.getGradientSameSize(direction);	
}


CIPPImage<Ipp32f> getGradientMagnitude(const CIPPImage<Ipp16u> &image)
{
	CIPPImage<Ipp32f> image32f = convert_16u32f(image);
	return image32f.getGradientMagnitude();	
}


/**
 * IppStatus getDivergence(const Ipp8u* pSrc, IppiSize sizeSrc, Ipp16u* &pDst, IppiSize *pSizeDst, int nChannels)
 * get the divergence of the source image, the divergence of the image is the sum of the d^2(x) + d^2(y)
 *
 * @param pSrc pointer to the source image
 * @param sizeSrc size of the source image
 * @param pDst reference pointer to the destination image
 * @param pSizeDst pointer to the size of the destination image
 * @param nChannels number of channels
 * @return IppStatus
 **/
CIPPImage<Ipp32f> getDivergence(const CIPPImage<Ipp16u> &image)
{
	return getDivergence(getGradient(image, IPPIMAGE_HORIZONTAL), 
		getGradient(image, IPPIMAGE_VERTICAL));
}


CIPPImage<Ipp32f> getDivergence(const CIPPImage<Ipp32f> & Dx, const CIPPImage<Ipp32f> & Dy)
{
	// get the divergent field using backward differencing
	// and add divx and divy
	CIPPImage<Ipp32f> DDx = Dx.getGradient(IPPIMAGE_HORIZONTAL);
	CIPPImage<Ipp32f> DDy = Dy.getGradient(IPPIMAGE_VERTICAL);

	IppiSize sizeResult = {DDx._size.width, DDy._size.height};
	CIPPImage<Ipp32f> result(sizeResult, 1);

	// get the divergent field
	ippiAdd_32f_C1R(DDx._imageData + DDx._size.width , DDx._size.width * 4, 
					DDy._imageData + 1, DDy._size.width * 4, 
					result._imageData, result._size.width * 4, result._size);

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
CIPPImage<Ipp16u> CIPPImage<Ipp16u>::rotateAroundCenter(double angle, double * pXShift, double * pYShift, int interpolation) const
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

	CIPPImage<Ipp16u> result(sizeResult, _nChannels, 0);

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
CIPPImage<Ipp16u> CIPPImage<Ipp16u>::rotateCenter(double angle, double xCenter, double yCenter, int interpolation) const
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

	CIPPImage<Ipp16u> result(sizeResult, _nChannels, 0);

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
CIPPImage<Ipp16u> CIPPImage<Ipp16u>::getRotatedSquarePatch(IppiPoint rotationAxis, double angleInDegree, int radius) const
{

	// the large radius needed for the rotation
	int radiusLarge = int(ceil(radius * 1.414213562));

	IppiSize sizePatch = {2*radius + 1, 2*radius + 1}; 
	IppiSize sizePatchLarge = {2*radiusLarge+1, 2*radiusLarge+1};

	// CIPPImage<Ipp16u> result(sizePatch, _nChannels, 0);
	// not initialized if failure
	CIPPImage<Ipp16u> result;

	IppiPoint start;
	start.x = rotationAxis.x - radiusLarge;
	start.y = rotationAxis.y - radiusLarge;

	if(start.x >= 0 && start.y >= 0 && 
		start.x + sizePatchLarge.width <= _size.width && 
		start.y + sizePatchLarge.height <= _size.height)
	{
		CIPPImage<Ipp16u> patchLarge = getRegion(start, sizePatchLarge);

		double xShift, yShift;
		CIPPImage<Ipp16u> patchLargeRotated = patchLarge.rotateAroundCenter(angleInDegree, 
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
CIPPImage<Ipp16u> CIPPImage<Ipp16u>::mirror(IppiAxis flip) const
{
	CIPPImage<Ipp16u> result(_size, _nChannels);
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
int CIPPImage<Ipp16u>::countInRange(IppiPoint start, IppiSize sizeRegion, Ipp16u lowerBound, Ipp16u upperBound) const
{
	int counts;
	CIPPImage<Ipp32f> this32f = convert_16u32f(*this);

	int offset = start.x + start.y * _size.width;
	ippiCountInRange_32f_C1R(this32f._imageData + offset, _size.width*4, sizeRegion, &counts, (Ipp32f)lowerBound, (Ipp32f)upperBound);

	return counts;
}

template<>
int CIPPImage<Ipp16u>::countInRange(Ipp16u lowerBound, Ipp16u upperBound) const
{
	int counts;
	CIPPImage<Ipp32f> this32f = convert_16u32f(*this);

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
CIPPImage<Ipp16u> remap(const CIPPImage<Ipp16u> &source, 
					   const CIPPImage<Ipp32f> &xMap, const CIPPImage<Ipp32f> &yMap,
					   IppiSize resultSize, int interpolation)
{
	IppStatus status;

	IppiSize size = source._size;
	CIPPImage<Ipp32f> sourceTemp = convert_16u32f(source);
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

	if (status)
		displayStatus(status);
	return convert_32f16u(resultTemp);
}

template<>
CIPPImage<Ipp16u> CIPPImage<Ipp16u>::remap(const CIPPImage<Ipp32f> &xMap, const CIPPImage<Ipp32f> &yMap,
					   IppiSize resultSize, int interpolation) const
{
	IppStatus status;

	CIPPImage<Ipp32f> sourceTemp = convert_16u32f(*this);
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

	if (status)
		displayStatus(status);
	return convert_32f16u(resultTemp);
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
CIPPImage<Ipp16u> alphaComp(const CIPPImage<Ipp16u> &A, const CIPPImage<Ipp16u> &B, IppiAlphaType alphaType)
{
	assert(A._size.width == B._size.width && A._size.height == B._size.height &&
		A._nChannels == B._nChannels);

	IppiSize size = A._size;
	int nChannels = A._nChannels;

	CIPPImage<Ipp16u> result(size, nChannels);

	ippiAlphaComp_16u_AC1R((const Ipp16u*)A._imageData, size.width*2,
		(const Ipp16u*)B._imageData, size.width*2,
		(Ipp16u*)result._imageData, size.width*2, size,
		alphaType);
	
	return result;
}


template<>
CIPPImage<Ipp16u> operator* (const CIPPImage<Ipp16u> &image1, const CIPPImage<Ipp16u> &image2)
{
	assert(image1._size.width == image2._size.width && image1._size.height == image2._size.height
		&& image1._nChannels == image2._nChannels);

	IppiSize size = image1._size;

	CIPPImage<Ipp32f> result32f(image1._size, image1._nChannels);
	CIPPImage<Ipp32f> image132f = convert_16u32f(image1);
	CIPPImage<Ipp32f> image232f = convert_16u32f(image2);


	ippiMul_32f_C1R(result32f._imageData, size.width*4, image132f._imageData, size.width*4, 
			image232f._imageData, size.width*4, size);

	return convert_32f16u(result32f);
}



// take the highest value of the image and subtract the entire image
template<>
CIPPImage<Ipp16u> CIPPImage<Ipp16u>::invert() const
{
	CIPPImage<Ipp16u> result(_size, _nChannels);
	CIPPImage<Ipp32f> image32f = convert_16u32f(*this);

	Ipp32f max;

	// get the max and min of the image
	// rescale the image so we can display it
	// ippiMin_8u_C1R(image, _size.width, _size, &min);
	ippiMax_32f_C1R(image32f._imageData, _size.width*4, _size, &max);

	for(int y = 0; y < _size.height; y++)
	{
		for(int x = 0; x < _size.width; x++)
		{
			result.setPixel(x, y, (Ipp16u)max - getPixel(x, y));
		}
	}
	return result;
}

template<>
CIPPImage<Ipp8u> CIPPImage<Ipp16u>::invertBinaryMask() const
{
	assert(_nChannels == 1);
	CIPPImage<Ipp8u> result(_size, _nChannels, 0);
	
	for(int i = 0; i < _size.height * _size.width; i++)
		if(_imageData[i])	result._imageData[i] = 1;
		else				result._imageData[i] = 0;
	return result;
}

template<>
inline CIPPImage<Ipp16u> CIPPImage<Ipp16u>::RGBToGray() const
{
	if(1 == _nChannels) return (*this);

	CIPPImage<Ipp16u> result(_size, 1);
	if(3 == _nChannels)
		ippiRGBToGray_16u_C3C1R(_imageData, 
			_size.width*_nChannels*2,
			result._imageData,
			result._size.width*2,
			_size);
	else // if(4 == _nChannels)
		ippiRGBToGray_16u_AC4C1R(_imageData, 
			_size.width*_nChannels*2,
			result._imageData,
			result._size.width*2,
			_size);

	return result;
}

template<>
CIPPImage<Ipp8u> CIPPImage<Ipp16u>::makeMask(Ipp16u threshold) const
{
	CIPPImage<Ipp8u> result(_size, 1, Ipp16u(0));
	if(_nChannels == 1)
	{
		for(int i = 0; i < _size.width*_size.height; i++)
			if(threshold < _imageData[i])
				result._imageData[i] = 1;
	}
	else
	{	
		CIPPImage<Ipp16u> imageGray = RGBToGray();
		for(int i = 0; i < _size.width*_size.height; i++)
			if(threshold < imageGray._imageData[i])
				result._imageData[i] = 1;
	}
	return result;
}


template<>
CIPPImage<Ipp16u> CIPPImage<Ipp16u>::getChannel(int channel) const
{
	assert(1 < _nChannels);
	assert(0 <= channel && channel < _nChannels);

	CIPPImage<Ipp16u> result(_size, 1);
	//IppStatus status = 
	ippiCopy_16u_C3C1R(_imageData+channel, 
		_size.width*_nChannels*2, result._imageData, _size.width*2, _size);
	
	return result;
}
