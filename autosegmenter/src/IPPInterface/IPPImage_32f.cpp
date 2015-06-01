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
CIPPImage<Ipp32f>::CIPPImage(IppiSize size, int nChannels)
{
	if(nChannels == 1)
		_imageData = ippiMalloc_32f_C1	( size.width, size.height, &_stepBytes);
	else if(nChannels == 3)
		_imageData = ippiMalloc_32f_C3	( size.width, size.height, &_stepBytes);
	else // if(img->nChannels == 4)
		_imageData = ippiMalloc_32f_AC4   ( size.width, size.height, &_stepBytes);
	
	_size = size;
	_nChannels = nChannels;
}

template<>
CIPPImage<Ipp32f>::CIPPImage(int width, int height, int nChannels )
{

	if(nChannels == 1)
		_imageData = ippiMalloc_32f_C1	( width, height, &_stepBytes);
	else if(nChannels == 3)
		_imageData = ippiMalloc_32f_C3	( width, height, &_stepBytes);
	else // if(img->nChannels == 4)
		_imageData = ippiMalloc_32f_AC4   ( width, height, &_stepBytes);

	_size.width = width;
	_size.height = height;
	_nChannels = nChannels;
}

template<>
CIPPImage<Ipp32f>::CIPPImage(IppiSize size, int nChannels, const Ipp32f value)
{
	_size = size;
	_nChannels = nChannels;

	if(_nChannels == 1)
	{
		_imageData = ippiMalloc_32f_C1	( _size.width, _size.height, &_stepBytes);
		ippiSet_32f_C1R(value, _imageData, _size.width*_nChannels*4, _size);
	}
	else if(_nChannels == 3)
	{
		Ipp32f values[3] = {value, value, value};
		_imageData = ippiMalloc_32f_C3	( _size.width, _size.height, &_stepBytes);
		ippiSet_32f_C3R(values, _imageData, _size.width*_nChannels*4, _size);
	}
	else // if(img->nChannels == 4)
	{
		Ipp32f values[4] = {value, value, value, value};
		_imageData = ippiMalloc_32f_AC4   ( _size.width, _size.height, &_stepBytes);
		ippiSet_32f_AC4R(values, _imageData, _size.width*_nChannels*4, _size);
	}
}

template<>
CIPPImage<Ipp32f>::CIPPImage(int width, int height, int nChannels, const Ipp32f value )
{
	_size.width = width;
	_size.height = height;
	_nChannels = nChannels;

	if(_nChannels == 1)
	{
		_imageData = ippiMalloc_32f_C1	( _size.width, _size.height, &_stepBytes);
		ippiSet_32f_C1R(value, _imageData, _size.width*_nChannels*4, _size);
	}
	else if(_nChannels == 3)
	{
		Ipp32f values[3] = {value, value, value};
		_imageData = ippiMalloc_32f_C3	( _size.width, _size.height, &_stepBytes);
		ippiSet_32f_C3R(values, _imageData, _size.width*_nChannels*4, _size);
	}
	else // if(img->nChannels == 4)
	{
		Ipp32f values[4] = {value, value, value, value};
		_imageData = ippiMalloc_32f_AC4   ( _size.width, _size.height, &_stepBytes);
		ippiSet_32f_AC4R(values, _imageData, _size.width*_nChannels*4, _size);
	}
}


template<>
CIPPImage<Ipp32f>::CIPPImage(int width, int height, int nChannels, const Ipp32f* data )
{	
	_size.width = width;
	_size.height = height;
	_nChannels = nChannels;

	if(_nChannels == 1)
	{
		// memory allocation for image
		_imageData = ippiMalloc_32f_C1( _size.width, _size.height, &_stepBytes);

		// the pointer of our OpenCV data is our IPP image pointer
		ippiCopy_32f_C1R ( data, width*_nChannels*4 , _imageData, _size.width * _nChannels*4, _size );

		// function ippiMalloc allocates a memory block aligned to a 32-byte 
		// boundary for elements of different data types.
		// step size should be stepBytes
	}
	else if(_nChannels == 3)
	{
		_imageData = ippiMalloc_32f_C3( _size.width, _size.height, &_stepBytes);
		ippiCopy_32f_C3R ( data, width*_nChannels*4, _imageData, _size.width * _nChannels*4 , _size );
	}
	else // if(img->_nChannels == 4)
	{
		_imageData = ippiMalloc_32f_AC4( _size.width, _size.height, &_stepBytes);
		ippiCopy_32f_AC4R( data, width*_nChannels*4, _imageData, _size.width * _nChannels*4, _size );
	}
}



/**
 * copy constructor
 **/
template<>
CIPPImage<Ipp32f>::CIPPImage(const CIPPImage<Ipp32f> &refImage)
{
	assert(&refImage != NULL);
	assert(refImage.isInitialized());

	_nChannels = refImage._nChannels;
	_size = refImage._size;

	if(_nChannels == 1)
	{
		_imageData = ippiMalloc_32f_C1	( _size.width, _size.height, &_stepBytes);
		ippiCopy_32f_C1R ( refImage._imageData, _size.width * 4 *_nChannels, _imageData, _size.width * 4 * _nChannels, _size );
	}
	else if(_nChannels == 3)
	{
		_imageData = ippiMalloc_32f_C3	( _size.width, _size.height, &_stepBytes);
		ippiCopy_32f_C3R ( refImage._imageData, _size.width * 4 * _nChannels, _imageData, _size.width * 4 * _nChannels, _size );
	}
	else // if(img->nChannels == 4)
	{
		_imageData = ippiMalloc_32f_AC4   ( _size.width, _size.height, &_stepBytes);
		ippiCopy_32f_AC4R ( refImage._imageData, _size.width * 4 * _nChannels, _imageData, _size.width * 4 * _nChannels, _size );
	}
}


// ------------------------------------------------------
// Overloaded Operators 
// ------------------------------------------------------
// assignment operator
template<>
CIPPImage<Ipp32f>& CIPPImage<Ipp32f>::operator = (const CIPPImage<Ipp32f> &refImage)
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
		_imageData = ippiMalloc_32f_C1	( _size.width, _size.height, &_stepBytes);
		ippiCopy_32f_C1R ( refImage._imageData, _size.width * 4 *_nChannels, _imageData, _size.width * 4 * _nChannels, _size );
	}
	else if(_nChannels == 3)
	{
		_imageData = ippiMalloc_32f_C3	( _size.width, _size.height, &_stepBytes);
		ippiCopy_32f_C3R ( refImage._imageData, _size.width * 4 * _nChannels, _imageData, _size.width * 4 * _nChannels, _size );
	}
	else // if(img->nChannels == 4)
	{
		_imageData = ippiMalloc_32f_AC4   ( _size.width, _size.height, &_stepBytes);
		ippiCopy_32f_AC4R ( refImage._imageData, _size.width * 4 * _nChannels, _imageData, _size.width * 4 * _nChannels, _size );
	}
	return (*this);
}




template<>	
CIPPImage<Ipp32f> CIPPImage<Ipp32f>::operator+ (const CIPPImage<Ipp32f> &refImage) const
{
	CIPPImage<Ipp32f> result(_size, _nChannels);

	ippiAdd_32f_C1R(refImage._imageData, refImage._size.width*_nChannels*4, _imageData, _size.width*4, 
						result._imageData, _size.width*_nChannels*4, _size);  
	return result;
}

template<>	
CIPPImage<Ipp32f> CIPPImage<Ipp32f>::operator- (const CIPPImage<Ipp32f> &refImage) const
{
	assert(_size.width == refImage._size.width && _size.height == refImage._size.height);
	CIPPImage<Ipp32f> result(_size, _nChannels);

	ippiSub_32f_C1R(refImage._imageData, refImage._size.width*_nChannels*4, _imageData, _size.width*_nChannels*4, 
						result._imageData, _size.width*_nChannels*4, _size);  
	return result;
}

// notice that mask must be an Ipp8u image with value between 0 and 1
template<>
CIPPImage<Ipp32f> CIPPImage<Ipp32f>::operator* (const CIPPImage<Ipp32f> &mask)const
{
	CIPPImage<Ipp32f> result(_size, _nChannels);
	ippiMul_32f_C1R(_imageData, _size.width*4, mask._imageData, mask._size.width*4, 
					result._imageData, result._size.width*4, mask._size);
	return result;
}


template<>
CIPPImage<Ipp32f> CIPPImage<Ipp32f>::operator* (Ipp32f value)const
{
	assert(NULL != _imageData);
	CIPPImage<Ipp32f> result(_size, _nChannels);

	if(_nChannels == 1)
	{
		ippiMulC_32f_C1R(_imageData, _size.width*_nChannels*4, (Ipp32f)value, 
					result._imageData, result._size.width*_nChannels*4, result._size);
	}
	else if(_nChannels == 3)
	{
		Ipp32f values[3] = {value, value, value};
		ippiMulC_32f_C3R(_imageData, _size.width*_nChannels*4, values, 
					result._imageData, result._size.width*_nChannels*4, result._size);
	}
	else // if(img->nChannels == 4)
	{
		Ipp32f values[4] = {value, value, value, value};
		ippiMulC_32f_AC4R(_imageData, _size.width*_nChannels*4, values, 
					result._imageData, result._size.width*_nChannels*4, result._size);
	}

	return result;
}


template<>
CIPPImage<Ipp32f> CIPPImage<Ipp32f>::operator- (void) const
{
	return ((*this)*(-1));
}

template<> 
CIPPImage<Ipp32f> operator+ (const CIPPImage<Ipp32f> &image, Ipp32f value)
{
	CIPPImage<Ipp32f> result(image._size, 1, 0);

	ippiAddC_32f_C1R(image._imageData, image._size.width*4,
		value, result._imageData, result._size.width*4, image._size);

	return result;
}

template<> 
CIPPImage<Ipp32f> operator- (const CIPPImage<Ipp32f> &image, Ipp32f value)
{
	CIPPImage<Ipp32f> result(image._size, 1, 0);

	ippiSubC_32f_C1R(image._imageData, image._size.width*4,
		value, result._imageData, result._size.width*4, image._size);

	return result;
}

template<> 
CIPPImage<Ipp32f> operator+ (Ipp32f value, const CIPPImage<Ipp32f> &image)
{
	CIPPImage<Ipp32f> result(image._size, 1, 0);

	ippiAddC_32f_C1R(image._imageData, image._size.width*4,
		value, result._imageData, result._size.width*4, image._size);

	return result;
}

template<> 
CIPPImage<Ipp32f> operator- (Ipp32f value, const CIPPImage<Ipp32f> &image)
{
	CIPPImage<Ipp32f> result = -image;
	// in place operation
	ippiAddC_32f_C1IR(value, result._imageData, 
		result._size.width*4, result._size);

	return result;
}



// multiply the mask with the image at the center
// TODO: make sure the mask and the original image agrees with size,
// that is, no out of boundary multiplication
template<>
CIPPImage<Ipp32f> CIPPImage<Ipp32f>::multiplyCenter (const CIPPImage<Ipp32f> &mask, IppiPoint center)
{
	// currently assume, the mask located at the center of the image
	// is contained entirely inside of the image

	CIPPImage<Ipp32f> result(_size, _nChannels, 0);
	int xStart = int(center.x - ceil(double(mask._size.width)/2));
	int yStart = int(center.y - ceil(double(mask._size.width)/2));

	if(xStart >= 0 && yStart >= 0 && 
		xStart + mask._size.width <= _size.width && yStart+mask._size.height <=_size.height)
	{
		long offset = xStart + yStart * _size.width;
		ippiMul_32f_C1R(_imageData + offset, _size.width*_nChannels*4, 
			mask._imageData, mask._size.width*_nChannels*4,
			result._imageData+offset, result._size.width*_nChannels*4, mask._size);
	}
	return result;
}


template<>
CIPPImage<Ipp32f> CIPPImage<Ipp32f>::operator/ (double val) const
{
	assert(NULL != _imageData);
	float value = (float) val;
	CIPPImage<Ipp32f> result(_size, _nChannels);

	if(_nChannels == 1)
	{
		ippiDivC_32f_C1R(_imageData, _size.width*_nChannels*4, (Ipp32f)value, 
					result._imageData, result._size.width*_nChannels*4, result._size);
	}
	else if(_nChannels == 3)
	{
		Ipp32f values[3] = {value, value, value};
		ippiDivC_32f_C3R(_imageData, _size.width*_nChannels*4, values, 
					result._imageData, result._size.width*_nChannels*4, result._size);
	}
	else // if(img->nChannels == 4)
	{
		Ipp32f values[4] = {value, value, value, value};
		ippiDivC_32f_AC4R(_imageData, _size.width*_nChannels*4, values, 
					result._imageData, result._size.width*_nChannels*4, result._size);
	}

	//for(int i = 0; i < _size.width * _size.height; i++)
	//{
	//	cout<< "(" << _imageData[i*3] << ", " << _imageData[i*3+1] << ", " << _imageData[i*3+2]<< ") - "; 
	//	cout<< "(" << result._imageData[i*3] << ", " << result._imageData[i*3+1] << ", " << result._imageData[i*3+2]<< ")\n"; 
	//}

	return result;
}


template<>
IppStatus CIPPImage<Ipp32f>::set(const Ipp32f value)
{
	if(NULL == _imageData)
		return ippStsNullPtrErr;

	if(_nChannels == 1)
	{
		return ippiSet_32f_C1R(value, _imageData, _size.width*4, _size);
	}
	else if(_nChannels == 3)
	{
		Ipp32f values[3] = {value, value, value};
		return ippiSet_32f_C3R(values, _imageData, _size.width*_nChannels*4, _size);
	}
	else // if(img->nChannels == 4)
	{
		Ipp32f values[4] = {value, value, value, value};
		return ippiSet_32f_AC4R(values, _imageData, _size.width*_nChannels*4, _size);
	}
}


template<>
IppStatus CIPPImage<Ipp32f>::set(const Ipp32f value, const CIPPImage<Ipp8u> &mask)
{
	assert(mask._size.height == _size.height && mask._size.width == _size.width);

	if(NULL == _imageData)
		return ippStsNullPtrErr;

	if(_nChannels == 1)
	{
		return ippiSet_32f_C1MR(value, _imageData, _size.width*4, _size, mask._imageData, _size.width);
	}
	else if(_nChannels == 3)
	{
		Ipp32f values[3] = {value, value, value};
		return ippiSet_32f_C3R(values, _imageData, _size.width*_nChannels*4, _size);
	}
	else // if(img->nChannels == 4)
	{
		Ipp32f values[4] = {value, value, value, value};
		return ippiSet_32f_AC4R(values, _imageData, _size.width*_nChannels*4, _size);
	}
}


template<>
IppStatus CIPPImage<Ipp32f>::setRegion(const Ipp32f value, IppiPoint start, IppiSize sizeROI)
{
	if(NULL == _imageData)
		return ippStsNullPtrErr;

	long offset = (start.x + start.y * _size.width)*_nChannels;

	if(_nChannels == 1)
	{	return ippiSet_32f_C1R(value, _imageData + offset, _size.width*4, sizeROI);
	}
	else if(_nChannels == 3)
	{	Ipp32f values[3] = {value, value, value};
		return ippiSet_32f_C3R(values, _imageData + offset, _size.width*_nChannels*4, sizeROI);
	}
	else // if(img->nChannels == 4)
	{	Ipp32f values[4] = {value, value, value, value};
		return ippiSet_32f_AC4R(values, _imageData + offset, _size.width*_nChannels*4, sizeROI);
	}
}


template<>
IppStatus CIPPImage<Ipp32f>::setRegionMasked(const Ipp32f value, IppiPoint start, IppiSize sizeROI, 
						const CIPPImage<Ipp8u> &mask, IppiPoint maskStart)
{
	if(NULL == _imageData)
		return ippStsNullPtrErr;

	long offset = (start.x + start.y * _size.width)*_nChannels;
	long maskOffset = maskStart.x + maskStart.y * mask._size.width;

	if(_nChannels == 1)
	{	
		return ippiSet_32f_C1MR(value, _imageData+offset, _size.width*_nChannels*4, sizeROI, 
			mask._imageData+maskOffset, mask._size.width);
	}
	else if(_nChannels == 3)
	{	Ipp32f values[3] = {value, value, value};
		return ippiSet_32f_C3MR(values, _imageData+offset, _size.width*_nChannels*4, sizeROI, 
			mask._imageData+maskOffset, mask._size.width);
	}
	else // if(img->nChannels == 4)
	{	Ipp32f values[4] = {value, value, value, value};
		return ippiSet_32f_AC4MR(values, _imageData+offset, _size.width*_nChannels*4, sizeROI, 
			mask._imageData+maskOffset, mask._size.width);
	}
}


template<>
CIPPImage<Ipp32f> CIPPImage<Ipp32f>::getRegion(IppiPoint start, IppiSize sizeROI) const
{

	CIPPImage<Ipp32f> result(sizeROI, _nChannels, 0);
	long offset = (start.x + start.y * _size.width)*_nChannels;

	if(_nChannels == 1)
	{	ippiCopy_32f_C1R(_imageData + offset, _size.width*4, 
					result._imageData, result._size.width*4, sizeROI);
	}
	else if(_nChannels == 3)
	{	
		ippiCopy_32f_C3R(_imageData + offset, _size.width*_nChannels*4, 
					result._imageData, result._size.width*_nChannels*4, sizeROI);
	}
	else // if(img->nChannels == 4)
	{	
		ippiCopy_32f_AC4R(_imageData + offset, _size.width*_nChannels*4, 
					result._imageData, result._size.width*_nChannels*4, sizeROI);
	}
	return result;
}


template<>
IppStatus CIPPImage<Ipp32f>::copyRegion(IppiPoint start, const CIPPImage<Ipp32f> &patch, IppiPoint patchStart, IppiSize sizeROI)
{
	IppStatus status;
	if(NULL == _imageData)
		return ippStsNullPtrErr;

	long offset = (start.x + start.y * _size.width)*_nChannels;
	long patchOffset = (patchStart.x + patchStart.y * patch._size.width)*_nChannels;

	if(_nChannels == 1)
	{	status = ippiCopy_32f_C1R(patch._imageData+patchOffset, patch._size.width * 4, 
									_imageData + offset, _size.width * 4, sizeROI);
	}
	else if(_nChannels == 3)
	{	
		status = ippiCopy_32f_C3R(patch._imageData+patchOffset, patch._size.width * _nChannels * 4, 
									_imageData + offset, _size.width * _nChannels * 4, sizeROI);
	}
	else // if(img->nChannels == 4)
	{	
		status = ippiCopy_32f_AC4R(patch._imageData+patchOffset, patch._size.width * _nChannels * 4, 
									_imageData + offset, _size.width * _nChannels * 4, sizeROI);
	}
	return status;
}

template<>
IppStatus CIPPImage<Ipp32f>::copyReplicateBorder(IppiPoint start, IppiSize sizeROI, 
												 const CIPPImage<Ipp32f> &src, IppiPoint srcStart, IppiSize sizeSrcROI, 
												 int topBorderWidth, int leftBorderWidth)
{
	if(NULL == _imageData)
		return ippStsNullPtrErr;

	long offset = (start.x + start.y * _size.width)*_nChannels;
	long srcOffset = (srcStart.x + srcStart.y * src._size.width)*_nChannels;

	return ippiCopyReplicateBorder_32s_C1R(
			(Ipp32s*)src._imageData+srcOffset, src._size.width * _nChannels*4, sizeSrcROI, 
			(Ipp32s*)_imageData + offset, _size.width * _nChannels*4, sizeROI,
			topBorderWidth, leftBorderWidth);
	//else if(_nChannels == 3)
	//{	
	//	return ippiCopyReplicateBorder_32s_C3R(
	//		(Ipp32s*)src._imageData+srcOffset, src._size.width * _nChannels*4, sizeSrcROI, 
	//		(Ipp32s*)_imageData + offset, _size.width * _nChannels*4, sizeSrcROI,
	//		topBorderWidth, leftBorderWidth);
	//}
	//else // if(img->nChannels == 4)
	//{	
	//	return ippiCopyReplicateBorder_32s_AC4R(
	//		(Ipp32s*)src._imageData+srcOffset, src._size.width * _nChannels*4, sizeSrcROI, 
	//		(Ipp32s*)_imageData + offset, _size.width * _nChannels*4, sizeSrcROI,
	//		topBorderWidth, leftBorderWidth);
	//}
}

// mask must be of the same size as the patch
//template<>
//IppStatus CIPPImage<Ipp32f>::copyRegionMask(IppiPoint start, const CIPPImage<Ipp32f> &patch, IppiPoint patchStart, 
//										   IppiSize sizeROI, const CIPPImage<Ipp32f> &mask)
//{
//	if(NULL == _imageData)
//		return ippStsNullPtrErr;
//
//	long offset = start.x + start.y * _size.width;
//	long patchOffset = patchStart.x + patchStart.y * patch._size.width;
//
//	if(_nChannels == 1)
//	{	
//		return ippiCopy_32f_C1MR(patch._imageData+patchOffset, patch._size.width, 
//			_imageData + offset, _size.width, sizeROI, mask._imageData, mask._size.width);
//	}
//	else if(_nChannels == 3)
//	{	
//		return ippiCopy_32f_C3MR(patch._imageData+patchOffset, patch._size.width, 
//			_imageData + offset, _size.width, sizeROI, mask._imageData, mask._size.width);
//	}
//	else // if(img->nChannels == 4)
//	{	
//		return ippiCopy_32f_AC4MR(patch._imageData+patchOffset, patch._size.width, 
//			_imageData + offset, _size.width, sizeROI, mask._imageData, mask._size.width);
//	}
//}

template<>
double CIPPImage<Ipp32f>::sum() const
{
	Ipp64f result = -1;
	if(1 == _nChannels)
	{
		ippiSum_32f_C1R(_imageData, _size.width*4, _size, &result, ippAlgHintNone);
	}
	return double(result);
}

template<>
double CIPPImage<Ipp32f>::sum(IppiPoint start, IppiSize sizeROI) const
{
	Ipp64f result = -1;

	long offset;
	if(1 == _nChannels)
	{
		offset = start.x + start.y * _size.width;
		ippiSum_32f_C1R(_imageData + offset, _size.width*4, sizeROI, &result, ippAlgHintNone);
	}

	return double(result);
}


template<>
Ipp32f CIPPImage<Ipp32f>::getMax(int *pIndexX, int *pIndexY) const
{
	Ipp32f value;

	if(1 == _nChannels)
	{
		ippiMaxIndx_32f_C1R(_imageData, _size.width*4, _size, &value, pIndexX, pIndexY);
	}
	//else if(3 == _nChannels)
	//{
	//	
	//}
	//else // if(4 == _nChannels)
	//{
	//}

	return value;
}

template<>
Ipp32f CIPPImage<Ipp32f>::getMin(int *pIndexX, int *pIndexY) const
{
	Ipp32f value;

	ippiMinIndx_32f_C1R(_imageData, _size.width*4, _size, &value, pIndexX, pIndexY); 
	return value;
}

template<>
Ipp32f CIPPImage<Ipp32f>::getMin(int *pIndexX, int *pIndexY, const CIPPImage<Ipp8u>& mask) const
{
	Ipp32f value;

	CIPPImage<Ipp32f> temp(*this);

	// since we always keep unwanted region under masked value 0
	// we need to invert that to set them to float max
	// so that they will never be selected as min
	temp.set(FLT_MAX, mask.invert());
	// 0 terms of the mask will be set to FLT_MAX, 1 terms will be set to 0
	
	ippiMinIndx_32f_C1R(temp._imageData, _size.width*4, _size, &value, pIndexX, pIndexY);
	return value;
}


template<>
CIPPImage<Ipp32f> CIPPImage<Ipp32f>::getGradient(int direction) const
{
	IppStatus status;
	IppiSize sizeKernel;
	IppiSize sizeResult;
	IppiPoint anchor;

	//allocateImage(temp, _size, 1);
	//ippiConvert_8u32f_C1R(_imageData, _size.width, temp, _size.width * 2, _size);

	// The output value is computed as a sum of neighbor pixels¡¯ values, with kernel matrix
	// elements used as weight factors. Note that summation formulas implement a
	// convolution operation, which means that kernel coefficients are used in inverse order.
	Ipp32f kernel[2] = {1, -1};

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
	CIPPImage<Ipp32f> result(sizeResult, 1);

	status = ippiFilter_32f_C1R(_imageData, _size.width * 4, result._imageData, 
			sizeResult.width * 4, sizeResult, kernel, sizeKernel, anchor);
	// printIpp(pDst, *pSizeDst);

	if (status)
		displayStatus(status);
	return result;
}


template<>
CIPPImage<Ipp32f> CIPPImage<Ipp32f>::getGradientSameSize(int direction) const
{
	CIPPImage<Ipp32f> gradientValid;
	CIPPImage<Ipp32f> temp1(_size, _nChannels, 0);
	CIPPImage<Ipp32f> temp2(_size, _nChannels, 0);

	gradientValid = getGradient(direction);
	
	// gradientValid.print();

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
CIPPImage<Ipp32f> CIPPImage<Ipp32f>::getGradientMagnitude() const
{
	CIPPImage<Ipp32f> Dx = getGradientSameSize(IPPIMAGE_HORIZONTAL);
	CIPPImage<Ipp32f> Dy = getGradientSameSize(IPPIMAGE_VERTICAL);

	CIPPImage<Ipp32f> result(_size, _nChannels, 0);
	float dx, dy;

	for(int y = 0; y < _size.height; y++)
	{
		for(int x = 0; x < _size.width; x++)
		{
			dx = Dx.getPixel(x, y);
			dy = Dy.getPixel(x, y);
			result.setPixel(x, y, sqrt(dx*dx + dy*dy)); 
		}
	}
	return result;
}



template<>
CIPPImage<Ipp32f> CIPPImage<Ipp32f>::getDivergence() const
{
	CIPPImage<Ipp32f> Dx = getGradient(IPPIMAGE_HORIZONTAL);
	CIPPImage<Ipp32f> Dy = getGradient(IPPIMAGE_VERTICAL);

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
 *
 * The function ippiResize is declared in the ippi.h file. This function resizes the source
 * image ROI by xFactor in the x direction and yFactor in the y direction. The image size
 * can be either reduced or increased in each direction, depending on the values of
 * xFactor, yFactor. The result is resampled using the interpolation method specified by
 * the interpolation parameter, and written to the destination image ROI.
 *
 * @param xFactor
 * @param yFactor
 * @param interpolation The type of interpolation to perform for image resampling.
 *		  IPPI_INTER_NN nearest neighbor interpolation
 *	      IPPI_INTER_LINEAR linear interpolation
 *        IPPI_INTER_CUBIC cubic interpolation
 *        IPPI_INTER_SUPER supersampling interpolation
 *
 **/
template<>
CIPPImage<Ipp32f> CIPPImage<Ipp32f>::resize(double xFactor, double yFactor, int interpolation) const
{
	IppiSize sizeResult;
	IppiRect srcROI;
	IppiRect destROI;
	IppStatus status;

	srcROI.x = 0;
	srcROI.y = 0;
	srcROI.width = _size.width;
	srcROI.height = _size.height;

	sizeResult.width = (int)(_size.width * xFactor);
	sizeResult.height = (int)(_size.height * yFactor);

	destROI.x = 0;
	destROI.y = 0;
	destROI.width = sizeResult.width;
	destROI.height = sizeResult.height;

	CIPPImage<Ipp32f> result(sizeResult, _nChannels, 0);

	switch(_nChannels)
	{
		// if only one channel
	case 1:
		int bufsize;
		status = ippiResizeGetBufSize(srcROI, destROI, 1,
			interpolation, &bufsize);
		if (status)
			break;
		Ipp8u *buf = ippsMalloc_8u(bufsize);
		status = ippiResizeSqrPixel_32f_C1R(_imageData, _size,
			_size.width*4, srcROI, result._imageData,
			destROI.width*4, destROI, xFactor, yFactor, 0, 0,
			interpolation, buf);
		ippsFree(buf);
		break;
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
CIPPImage<Ipp32f> CIPPImage<Ipp32f>::rotateAroundCenter(double angle, double * pXShift, double * pYShift, int interpolation) const
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

	CIPPImage<Ipp32f> result(sizeResult, _nChannels, 0);

	switch(_nChannels)
	{
		// if only one channel
	case 1:
		status = ippiRotate_32f_C1R(_imageData, _size, _size.width*4, rectS, 
			result._imageData, sizeResult.width*4, rectR, angle, 
			xShift - bound[0][0], yShift - bound[0][1], interpolation);
		// printf( " %d : %s\n", status, ippCoreGetStatusString( status));
		break;
	}

	* pXShift = xShift;
	* pYShift = yShift;

	if (status)
		displayStatus(status);
	return result;
}



template<>
CIPPImage<Ipp32f> CIPPImage<Ipp32f>::rotateCenter(double angle, double xCenter, double yCenter, int interpolation) const
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

	CIPPImage<Ipp32f> result(sizeResult, _nChannels, 0);

	switch(_nChannels)
	{
		// if only one channel
	case 1:
		status = ippiRotate_32f_C1R(_imageData, _size, _size.width*4, rectS, 
			result._imageData, sizeResult.width*4, rectR, angle, 
			xShift - bound[0][0], yShift - bound[0][1], interpolation);
		// printf( " %d : %s\n", status, ippCoreGetStatusString( status));
		break;
	}

	if (status)
		displayStatus(status);
	return result;
}



template<>
CIPPImage<Ipp32f> CIPPImage<Ipp32f>::getRotatedSquarePatch(IppiPoint rotationAxis, double angleInDegree, int radius) const
{

	// the large radius needed for the rotation
	int radiusLarge = int(ceil(radius * 1.414213562));

	IppiSize sizePatch = {2*radius + 1, 2*radius + 1}; 
	IppiSize sizePatchLarge = {2*radiusLarge+1, 2*radiusLarge+1};

	// CIPPImage<Ipp32f> result(sizePatch, _nChannels, 0);
	// not initialized if failure
	CIPPImage<Ipp32f> result;

	IppiPoint start;
	start.x = rotationAxis.x - radiusLarge;
	start.y = rotationAxis.y - radiusLarge;

	if(start.x >= 0 && start.y >= 0 && 
		start.x + sizePatchLarge.width <= _size.width && 
		start.y + sizePatchLarge.height <= _size.height)
	{
		CIPPImage<Ipp32f> patchLarge = getRegion(start, sizePatchLarge);

		double xShift, yShift;
		CIPPImage<Ipp32f> patchLargeRotated = patchLarge.rotateAroundCenter(angleInDegree, 
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
CIPPImage<Ipp32f> CIPPImage<Ipp32f>::mirror(IppiAxis flip) const
{
	CIPPImage<Ipp32f> result(_size, _nChannels);
	switch(_nChannels)
	{
	case 1:
		ippiMirror_32s_C1R((Ipp32s*)_imageData, _size.width*4, 
			(Ipp32s*)result._imageData, _size.width*4, _size, flip);
		break;
	case 3:
		ippiMirror_32s_C3R((Ipp32s*)_imageData, _size.width*_nChannels*4, 
			(Ipp32s*)result._imageData, _size.width*_nChannels*4, _size, flip);
		break;
	case 4:
		ippiMirror_32s_AC4R((Ipp32s*)_imageData, _size.width*_nChannels*4, 
			(Ipp32s*)result._imageData, _size.width*_nChannels*4, _size, flip);
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
int CIPPImage<Ipp32f>::countInRange(IppiPoint start, IppiSize sizeRegion, Ipp32f lowerBound, Ipp32f upperBound) const
{
	int counts;

	int offset = start.x + start.y * _size.width;
	ippiCountInRange_32f_C1R(_imageData + offset, _size.width*4, sizeRegion, &counts, lowerBound, upperBound);

	return counts;
}

template<>
int CIPPImage<Ipp32f>::countInRange(Ipp32f lowerBound, Ipp32f upperBound) const
{
	int counts;
	ippiCountInRange_32f_C1R(_imageData, _size.width*4, _size, &counts, lowerBound, upperBound);
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
CIPPImage<Ipp32f> remap(const CIPPImage<Ipp32f> &source, 
					   const CIPPImage<Ipp32f> &xMap, const CIPPImage<Ipp32f> &yMap,
					   IppiSize resultSize, int interpolation)
{
	IppStatus status;

	IppiSize size = source._size;
	CIPPImage<Ipp32f> result(resultSize, source._nChannels, 0);

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
	status = ippiRemap_32f_C1R(source._imageData, size, size.width*4, ROI,
		xMap._imageData, resultSize.width*4,
		yMap._imageData, resultSize.width*4,
		result._imageData, resultSize.width*4, resultSize, IPPI_INTER_CUBIC);

	//return CIPPImage<Ipp32f>(result);
	if (status)
		displayStatus(status);
	return result;
}

template<>
CIPPImage<Ipp32f> CIPPImage<Ipp32f>::remap(const CIPPImage<Ipp32f> &xMap, const CIPPImage<Ipp32f> &yMap,
					   IppiSize resultSize, int interpolation) const
{
	IppStatus status;

	CIPPImage<Ipp32f> result(resultSize, _nChannels, 0);

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
	status = ippiRemap_32f_C1R(_imageData, _size, _size.width*4, ROI,
		xMap._imageData, resultSize.width*4,
		yMap._imageData, resultSize.width*4,
		result._imageData, resultSize.width*4, resultSize, IPPI_INTER_CUBIC);

	//return CIPPImage<Ipp32f>(result);
	if (status)
		displayStatus(status);
	return result;
}




/**
 * interface for ippiMatchTemplate: 8u_C1R
 *
 * should be interface for ippiCrossCorrSame: 8u_C1R
 * but since IPP does not provide cross correlation (not normalized) in it's basic
 * image statistic functions, we will use ippiMatchTemplate from IPP vision functions.
 * We assume ippiMatchTemplate is slower than ippiCrossCorrSame_Norm
 *
 * The function ippiMatchTemplate is declared in the ippcv.h file. This function
 * implements a set of methods for finding regions in the image, which are similar to a
 * given template.
 * Given a source image ofW¡ÁH pixels and template of w¡Á h pixels, the resulting image is
 * of (W - w + 1) ¡Á (H - h + 1) pixels, and the pixel value in each certain location
 * (x, y) characterizes the similarity between the template and image rectangle with the
 * top-left corner at (x, y) and the right-bottom corner at (x + w - 1, y + h - 1). The
 * similarity can be calculated using different methods described below.
 *
 * 
 * @param src source image
 * @param tpl template image
 * @return CIPPImage<Ipp32f> cross correlation map with the same dimension as the source
 */
// ==================================================================================================
// HOWARD : 2007-06-08
// notice that in IPP 5.2
// All of the ippiMatchTemplate functions are removed from ippicv.h
// (ippiMatchTemplate_SqDiff_32f_C1R, ippiMatchTemplate_SqDiff_32f_C1R), so there is basically no
// unnormalized squareDifference left. So the following version is actually a normalized squareDifference.
// However, the Intel people did keep the only unnormalized version of CrossCorr, which is CrossCorrValid
// 
// ==================================================================================================
template<>
CIPPImage<Ipp32f> crossCorrelation(const CIPPImage<Ipp32f> &src, const CIPPImage<Ipp32f> &tpl)
{
	assert(src._imageData != NULL && tpl._imageData != NULL);
	assert(src._nChannels == tpl._nChannels);
	assert(src._nChannels == 1);
	IppiSize sizeResult = {src._size.width - tpl._size.width + 1, src._size.height - tpl._size.height + 1};
	IppiPoint start = {int(tpl._size.width/2.), int(tpl._size.height/2.)};
	IppiPoint origin = {0, 0};

	CIPPImage<Ipp32f> result(sizeResult, src._nChannels, 0);
	CIPPImage<Ipp32f> resultSameSize(src._size, src._nChannels, 1);

	if(src._nChannels == 1)
	{
		ippiCrossCorrValid_32f_C1R(src._imageData, 
										src._size.width*4*src._nChannels, 
										src._size, 
										tpl._imageData, 
										tpl._size.width*4*tpl._nChannels,
										tpl._size,
										result._imageData,
										sizeResult.width*4*src._nChannels);
	}
	else if(src._nChannels == 3)
	{
		// also rotate the valid region mask
	}
	else // if(img->nChannels == 4)
	{
		// also rotate the valid region mask
	}

	resultSameSize.copyRegion(start, result, origin, sizeResult);

	return resultSameSize;;
}

template<>
CIPPImage<Ipp32f> squaredDifferenceMasked_FFT_C1R(const CIPPImage<Ipp32f> &src, 
										   const CIPPImage<Ipp32f> &tpl, 
										   const CIPPImage<Ipp32f> &mask)
{
	IppiPoint startValid = {int(tpl._size.width/2.), int(tpl._size.height/2.)};
	IppiSize sizeValid = {src._size.width - tpl._size.width + 1, src._size.height - tpl._size.height + 1};

	CIPPImage<Ipp32f> result(src._size, src._nChannels, FLT_MAX);
	CIPPImage<Ipp32f> tplMasked = tpl*mask;
	CIPPImage<Ipp32f> constTerm(src._size, src._nChannels, Ipp32f((tplMasked*tplMasked).sum()));
	CIPPImage<Ipp32f> resultTemp = constTerm + crossCorrelation(src*src, mask) - crossCorrelation(src, tplMasked)*2;

	// need to move the image so that the new center coinside with the old center
	result.copyRegion(startValid, resultTemp, startValid, sizeValid);
	return result;
}


template<>
CIPPImage<Ipp32f> squaredDifferenceMasked_FFT(const CIPPImage<Ipp32f> &src, 
										   const CIPPImage<Ipp32f> &tpl, 
										   const CIPPImage<Ipp8u> &mask)
{
	//IppiPoint startValid = {int(tpl._size.width/2.), int(tpl._size.height/2.)};
	//IppiSize sizeValid = {src._size.width - tpl._size.width + 1, src._size.height - tpl._size.height + 1};
	assert(src._nChannels == 1 || src._nChannels == 3 );
	assert(src._nChannels == tpl._nChannels);

	CIPPImage<Ipp32f> mask32f(mask._size, mask._nChannels, 0);
	convert_8u32f(mask, mask32f);

	IppStatus status;
	if(src._nChannels == 1)
		return squaredDifferenceMasked_FFT_C1R(src, tpl, mask32f);
	else // if(src._nChannels == 3)
	{
		// allocate memory for all three channels.
		CIPPImage<Ipp32f> srcR(src._size, 1);
		CIPPImage<Ipp32f> srcG(src._size, 1);
		CIPPImage<Ipp32f> srcB(src._size, 1);
		CIPPImage<Ipp32f> tplR(tpl._size, 1);
		CIPPImage<Ipp32f> tplG(tpl._size, 1);
		CIPPImage<Ipp32f> tplB(tpl._size, 1);

		Ipp32f *srcDst[3] = {srcR._imageData, srcG._imageData, srcB._imageData};
		Ipp32f *tplDst[3] = {tplR._imageData, tplG._imageData, tplB._imageData};

		// get one channel out
		status = ippiCopy_32f_C3P3R(src._imageData, src._size.width * src._nChannels * 4,
			srcDst, src._size.width*4, src._size);
		if (status)
			displayStatus(status);
		status = ippiCopy_32f_C3P3R(tpl._imageData, tpl._size.width * tpl._nChannels * 4,
			tplDst, tpl._size.width*4, tpl._size);

		CIPPImage<Ipp32f> resultR = squaredDifferenceMasked_FFT_C1R(srcR, tplR, mask32f);
		CIPPImage<Ipp32f> resultG = squaredDifferenceMasked_FFT_C1R(srcG, tplG, mask32f);
		CIPPImage<Ipp32f> resultB = squaredDifferenceMasked_FFT_C1R(srcB, tplB, mask32f);

		if (status)
			displayStatus(status);
		return resultR + resultG + resultB;

	}
	//CIPPImage<Ipp32f> tplMasked = tpl*mask32f;
	//CIPPImage<Ipp32f> constTerm(src._size, src._nChannels, Ipp32f((tplMasked*tplMasked).sum()));
	//CIPPImage<Ipp32f> resultTemp = constTerm + crossCorrelation(src*src, mask32f) - crossCorrelation(src, tplMasked)*2;

	//// need to move the image so that the new center coinside with the old center
	//result.copyRegion(startValid, resultTemp, startValid, sizeValid);

	//return result;
}


template<>
CIPPImage<Ipp32f> CIPPImage<Ipp32f>::filterGauss(IppiMaskSize mask) const
{
	CIPPImage<Ipp32f> result(_size, _nChannels, 0);
	IppiSize sizeSrcBorder;
	IppiPoint startSrcBorder = {0, 0};
	IppiPoint startSrc = {0, 0};

	if(ippMskSize5x5 == mask)
	{
		sizeSrcBorder.height = _size.height + 4;
		sizeSrcBorder.width = _size.width + 4;
		CIPPImage<Ipp32f> sourceBorder(sizeSrcBorder, _nChannels, 0);
		sourceBorder.copyReplicateBorder(startSrcBorder, sizeSrcBorder, *this, startSrc, _size, 2, 2);

		int ROIOffset = sizeSrcBorder.width * 2 + 2;
		ippiFilterGauss_32f_C1R(sourceBorder._imageData + ROIOffset, sizeSrcBorder.width*4, result._imageData, _size.width*4, _size, mask);
	}
	else // ippMskSize3x3
	{
		sizeSrcBorder.height = _size.height + 2;
		sizeSrcBorder.width = _size.width + 2;
		CIPPImage<Ipp32f> sourceBorder(sizeSrcBorder, _nChannels, 0);
		sourceBorder.copyReplicateBorder(startSrcBorder, sizeSrcBorder, *this, startSrc, _size, 1, 1);

		int ROIOffset = sizeSrcBorder.width + 1;
		ippiFilterGauss_32f_C1R(sourceBorder._imageData + ROIOffset, sizeSrcBorder.width*4, result._imageData, _size.width*4, _size, mask);
	}
		
	return result;
}

template<>
CIPPImage<Ipp32f> CIPPImage<Ipp32f>::filterLaplace(IppiMaskSize mask) const
{
	CIPPImage<Ipp32f> result(_size, _nChannels, 0);
	IppiSize sizeSrcBorder;
	IppiPoint startSrcBorder = {0, 0};
	IppiPoint startSrc = {0, 0};

	if(ippMskSize5x5 == mask)
	{
		sizeSrcBorder.height = _size.height + 4;
		sizeSrcBorder.width = _size.width + 4;
		CIPPImage<Ipp32f> sourceBorder(sizeSrcBorder, _nChannels, 0);
		sourceBorder.copyReplicateBorder(startSrcBorder, sizeSrcBorder, *this, startSrc, _size, 2, 2);

		int ROIOffset = sizeSrcBorder.width * 2 + 2;
		ippiFilterLaplace_32f_C1R(sourceBorder._imageData + ROIOffset, sizeSrcBorder.width*4, result._imageData, _size.width*4, _size, mask);
	}
	else // ippMskSize3x3
	{
		sizeSrcBorder.height = _size.height + 2;
		sizeSrcBorder.width = _size.width + 2;
		CIPPImage<Ipp32f> sourceBorder(sizeSrcBorder, _nChannels, 0);
		sourceBorder.copyReplicateBorder(startSrcBorder, sizeSrcBorder, *this, startSrc, _size, 1, 1);

		int ROIOffset = sizeSrcBorder.width + 1;
		ippiFilterLaplace_32f_C1R(sourceBorder._imageData + ROIOffset, sizeSrcBorder.width*4, result._imageData, _size.width*4, _size, mask);
	}
		
	return result;
}


template<>
CIPPImage<Ipp32f> CIPPImage<Ipp32f>::RGBToLUV() const
{
	if(1 == _nChannels) return (*this);
	CIPPImage<Ipp32f> result(_size, _nChannels, Ipp32f(0));

	ippiRGBToLUV_32f_C3R(_imageData, 
		_size.width*_nChannels*4,
		result._imageData,
		_size.width*_nChannels*4,
		_size);

	//for(int i = 0; i < _size.width * _size.height; i++)
	//{
	//	cout<< "(" << _imageData[i*3] << ", " << _imageData[i*3+1] << ", " << _imageData[i*3+2]<< ") - "; 
	//	cout<< "(" << result._imageData[i*3] << ", " << result._imageData[i*3+1] << ", " << result._imageData[i*3+2]<< ")\n"; 
	//}

	return result;
}
	
template<>
CIPPImage<Ipp32f> CIPPImage<Ipp32f>::LUVToRGB() const
{
	if(1 == _nChannels) return (*this);
	CIPPImage<Ipp32f> result(_size, _nChannels, Ipp32f(0));

	ippiLUVToRGB_32f_C3R(_imageData, 
		_size.width*_nChannels*4,
		result._imageData,
		result._size.width*_nChannels*4,
		_size);
	
	return result;
}




template<>
inline CIPPImage<Ipp32f> CIPPImage<Ipp32f>::RGBToGray() const
{
	if(1 == _nChannels) return (*this);

	CIPPImage<Ipp32f> result(_size, 1);
	if(3 == _nChannels)
		ippiRGBToGray_32f_C3C1R(_imageData, 
			_size.width*_nChannels*4,
			result._imageData,
			result._size.width*4,
			_size);
	else // if(4 == _nChannels)
		ippiRGBToGray_32f_AC4C1R(_imageData, 
			_size.width*_nChannels*4,
			result._imageData,
			result._size.width*4,
			_size);	
	return result;
}


template<>
CIPPImage<Ipp8u> CIPPImage<Ipp32f>::makeMask(Ipp32f threshold) const
{
	CIPPImage<Ipp8u> result(_size, 1, Ipp8u(0));

	if(1 == _nChannels)
	{
		for(int x = 0; x < _size.width; x++)
			for(int y = 0; y < _size.height; y++)
				if(threshold < _imageData[x+y*_size.width])
					result._imageData[x+y*_size.width] = 1;
	}
	else
	{
		CIPPImage<Ipp32f> imageGray = RGBToGray();
		for(int x = 0; x < _size.width; x++)
			for(int y = 0; y < _size.height; y++)
				if(threshold < imageGray._imageData[x+y*_size.width])
					result._imageData[x+y*_size.width] = 1;
	}

	return result;
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
CIPPImage<Ipp32f> CIPPImage<Ipp32f>::morphOpenBorder(const Ipp8u *pMask, IppiSize maskSize, IppiPoint anchor)	const
{
	IppStatus status = ippStsNoErr;
	IppiMorphAdvState* pState = NULL;
	
	CIPPImage<Ipp32f> result(*this);

	// allocate and initialize the morphAdvState
	status = ippiMorphAdvInitAlloc_32f_C1R(&pState, _size, pMask, maskSize, anchor);
	if (status)
		displayStatus(status);
	status = ippiMorphOpenBorder_32f_C1R(_imageData, _size.width*_nChannels*4, result._imageData, _size.width*_nChannels*4, _size, ippBorderRepl, pState);

	ippiMorphAdvFree(pState);
	pState = NULL;

	if (status)
		displayStatus(status);
	return result;
}

template<>
CIPPImage<Ipp32f> CIPPImage<Ipp32f>::morphCloseBorder(const Ipp8u *pMask, IppiSize maskSize, IppiPoint anchor)	const
{
	IppStatus status = ippStsNoErr;
	IppiMorphAdvState* pState = NULL;
	
	CIPPImage<Ipp32f> result(*this);

	// allocate and initialize the morphAdvState
	status = ippiMorphAdvInitAlloc_32f_C1R(&pState, _size, pMask, maskSize, anchor);
	if (status)
		displayStatus(status);
	status = ippiMorphCloseBorder_32f_C1R(_imageData, _size.width*_nChannels*4, result._imageData, _size.width*_nChannels*4, _size, ippBorderRepl, pState);

	ippiMorphAdvFree(pState);
	pState = NULL;

	if (status)
		displayStatus(status);
	return result;
}



//The function ippiFilterGaussBorder is declared in the ippcv.h file. It operates with
//ROI (see Regions of Interest in Intel IPP). This function applies the Gaussian filter to the
//source image ROI pSrc. The kernel of this filter is the matrix of size
//kernelSize*kernelSize with the standard deviation sigma. The values of the elements
//of the Gaussian kernel are calculated according to the formula:
//and then are normalized. The anchor cell is the center of the kernel.
//borderType Type of border (see Borders); following values are possible:
//  ippBorderConst Values of all border pixels are set to constant.
//	ippBorderRepl Replicated border is used.
//	ippBorderWrap Wrapped border is used
//	ippBorderMirror Mirrored border is used
//	ippBorderMirrorR Mirrored border with replication is used
template<>
CIPPImage<Ipp32f> CIPPImage<Ipp32f>::filterGaussBorder(int kernelSize, Ipp32f sigma, IppiBorderType borderType, Ipp32f borderValue) const
{
	assert(1 == _nChannels);
	assert(isInitialized());

	Ipp8u * pBuffer;
	int bufferSize = 0;
	IppStatus status = ippStsNoErr;

	status = ippiFilterGaussGetBufferSize_32f_C1R(_size, kernelSize, & bufferSize);
	if (status)
		displayStatus(status);

	// The function requires the working buffer pBuffer whose size should be computed by the
	// function ippiFilterGaussGetBufferSize beforehand.
	pBuffer = new Ipp8u[bufferSize];

	CIPPImage<Ipp32f> result(*this);

	status = ippiFilterGaussBorder_32f_C1R(_imageData, _size.width*_nChannels*4, 
										   result._imageData, _size.width*_nChannels*4,
										   _size, kernelSize, sigma, borderType, borderValue, pBuffer);

	delete[] pBuffer;
	if (status)
		displayStatus(status);
	return result;
}


template<>
CIPPImage<Ipp32f> CIPPImage<Ipp32f>::getChannel(int channel) const
{
	assert(1 < _nChannels);
	assert(0 <= channel && channel < _nChannels);

	CIPPImage<Ipp32f> result(_size, 1);
	//IppStatus status = 
	ippiCopy_32f_C3C1R(_imageData+channel, 
		_size.width*_nChannels*4, result._imageData, _size.width*4, _size);
	
	return result;
}


// regional maxima and minima
template<>
CIPPImage<Ipp8u> CIPPImage<Ipp32f>::findPeaks3x3(Ipp32f threshold, vector<IppiPoint> &peaks, int maxPeakCount, IppiNorm norm, int border) const
{
	assert(1 == _nChannels);
	assert(peaks.size() == 0);
	IppStatus status;
	int bufferSize, peakCount;
	IppiPoint origin = {0, 0};

	// pad the image to the step size
	CIPPImage<Ipp32f> imagePad(_stepBytes/4, _size.height, 1, Ipp32f(0));
	imagePad.copyRegion(origin, *this, origin, _size);

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
	

