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
 * Terrain Synthesis by Example: Terrain Analysis\n
 * @file PPA.h
 *
 * Primary Authors: Howard Zhou (howardz@cc.gatech.edu)\n
 *
 *
 * ==================================================================\n
 * Profile-recognition and Polygon-breaking Algorithm
 *
 * @version 1.0\n
 *     Initial Revision
 * @date 2005-04-20
 * @author Howard Zhou
 *
 *
 * @todo
 * @bug
 * @warning 
 *
 * ===================================================================\n
 */

#ifndef ___INPAINTER__H___
#define ___INPAINTER__H___

// #pragma warning(disable : 4786)
#include "IPPInterface/IPPImage.h"
#include "math/Vector.h"

#include "ippcv.h"

/// @namespace using standard namespace
using namespace std;

// ==================================================================================
/**
 * @class CInpainter
 * @brief Profile-recognition and Polygon-breaking Algorithm
 * @ingroup Terrain Analysis
 *
 * @par requirements
 *
 * @par license
 * This code is absolutely free to use and modify. The code is provided "as is" with
 * no expressed or implied warranty. The author accepts no liability if it causes
 * any damage to your computer, causes your pet to fall ill, increases baldness
 * or makes your car start emitting strange noises when you start it up.
 * This code has no bugs, just undocumented features!
 *
 * 
 * @version 1.0\n
 *     Initial Revision
 * @date 2005-04-20
 * @author Howard Zhou
 *
 * 
 * @todo
 * @bug
 * @warning
 */
// ================================================================================

// ===============================================================================================
// The following section of code implements the following paper.
// 
// A. Criminisi, P. P¨¦rez, and K. Toyama. 
// Object removal by exemplar-based inpainting. Conf. on Computer Vision and Pattern Recognition, 
// CVPR, Madison, Wisconsin, 2003.
// http://research.microsoft.com/vision/cambridge/i3l/patchworks.htm
// ------------------------------------------------------------------------------------------------
class CInpainter
{
protected:
	// input image
	CIPPImage<Ipp8u>	_imageOriginal;
	CIPPImage<Ipp8u>	_maskOriginal;

	// output image
	CIPPImage<Ipp8u>	_image8u;
	CIPPImage<Ipp8u>	_mask8u;

	// intermediate image
	CIPPImage<Ipp8u>	_maskInverted8u;
	CIPPImage<Ipp32f>	_imageMasked32f;/// used for squared distance
	CIPPImage<Ipp8u>	_imageGray;		/// gray image
	CIPPImage<Ipp8u>	_sourceMap;		/// source map
	CIPPImage<Ipp32f>	_confidenceMap;	/// confidence map
	CIPPImage<Ipp32f>	_priorityMap;	/// priority map	
	CIPPImage<Ipp16s>	_Dx;			/// gradient map
	CIPPImage<Ipp16s>	_Dy;			
	CIPPImage<Ipp16s>	_Nx;			/// normal map of the moving front
	CIPPImage<Ipp16s>	_Ny;

	bool _isValid;
	IppiSize _size;
	IppiSize _sizeWindow;

public:
	enum InpainterType{
		INPAINTER_TELEA,	/// IPP_INPAINT_TELEA	Intel IPP's implemenation of Telea algorithm;
		INPAINTER_NS,		/// IPP_INPAINT_NS		Intel IPP's implementation of Navier-Stokes equation.
		INPAINTER_CRIMINISI	/// Howard's implementation of Criminisi, Perez, and Toyama's algorithm.
	};

	enum InpainterPixelType{
		INPAINTER_SOURCE = 0,
		INPAINTER_BOUNDARY = 127,
		INPAINTER_TARGET = 255
	};

	// ------------------------------------------------------
	// constructors and destructors
	// ------------------------------------------------------
	/// default constructor
	CInpainter(int windowSize = 4)
		:_isValid(false)
	{
		_sizeWindow.height = _sizeWindow.width =  windowSize;
	};
	
	// use member initialization list.
	CInpainter(const CIPPImage<Ipp8u> &image, 
		const CIPPImage<Ipp8u> &mask,
		int windowSize = 4)
		:_imageOriginal(image),
		_maskOriginal(mask)
	{
		assert(mask._size.height == image._size.height
			   && mask._size.width == image._size.width);
		_size			= image._size;
		_sizeWindow.height = _sizeWindow.width =  windowSize;
		_image8u		= _imageOriginal;
		_mask8u			= _maskOriginal;
		_isValid = true;
	};

	/// copy constructor
	//CInpainter(const CInpainter<T> & p){}
		
	/// destructor
	~CInpainter(){};

	// ------------------------------------------------------
	// Predicators
	// ------------------------------------------------------
	bool isValid() const { return _isValid; };

	// ------------------------------------------------------
	// Accessors and Mutators
	// ------------------------------------------------------
	// ------------------------------------------------------
	// Overloaded Operators 
	// ------------------------------------------------------
	// ------------------------------------------------------
	// Input, Output, Test
	// ------------------------------------------------------
	bool load(const char* filename, const char* maskFilename = NULL);
	bool save(const char* filename, bool type) const;

	// ------------------------------------------------------
	// Other operation
	// ------------------------------------------------------
	void	run				(InpainterType type = INPAINTER_TELEA);
	void	runInpaintIPP	(int flag);
	void	runCriminisi	(void);

	void	initSourceMap	(void);
	void	initConfidenceMap(void);

	void	setBoundary		(IppiPoint start, IppiSize roiSize);
	void	setPriorityMap	(IppiPoint start, IppiSize roiSize);

	float	getPriority		(int x, int y) const;
	float	getConfidence	(int x, int y) const;
	float	getData			(int x, int y) const;

	void	findMaxPriority	(int &xp, int &yp);
	void	findBestPatch	(int x, int y, int &xp, int &yp);
	
	void	update			(int x, int y, int xp, int yp);
	void	updateROI		(int x, int y);

	CIPPImage<Ipp8u> makeMask(const CIPPImage<Ipp8u> &image, const CVector3<int> &color);
	void	makeMaskedImage	(void);

	CIPPImage<Ipp8u> getResult(void){	
		assert(_isValid); 
		return _image8u;	
	}

	// ------------------------------------------------------
	// Friend functions
	// ------------------------------------------------------

private:
	// ------------------------------------------------------
	// private helpers
	// ------------------------------------------------------

	// main functional steps
	// ------------------------------------------------------

	// other helpers
	// ------------------------------------------------------

};

inline float CInpainter::getPriority(int x, int y) const
{
	assert(_isValid);
	return getConfidence(x, y)*getData(x, y);
}

// sum up the confidence from the window
inline float CInpainter::getConfidence(int x, int y) const 
{
	assert(_isValid);
	//double normalizationTerm = (_sizeWindow.width*2+1)*(_sizeWindow.height * 2+1);

	int height = _sizeWindow.height*2+1, width = _sizeWindow.width*2+1;
	IppiPoint p = {max(x-_sizeWindow.width, 0), max(y-_sizeWindow.height, 0)};
	int heightn = min(_size.height - p.y, height);
	int widthn = min(_size.width - p.x, width);

	// make sure the window does not go over the image boundary
	IppiSize roiSize = {widthn, heightn};
	return _confidenceMap.sum(p, roiSize)/(height*width);
}


inline bool CInpainter::save(const char* filename, bool type) const
{
	assert(_isValid && filename);
	IppStatus status = _image8u.save(filename, 1);
	return (status == ippStsNoErr);
}


inline void CInpainter::initConfidenceMap(void)
{
	assert(_isValid);
	for(int i=0; i <_size.width *_size.height; i++)
		if(INPAINTER_SOURCE == _mask8u._imageData[i])
			_confidenceMap._imageData[i] = 1; 
}


inline CIPPImage<Ipp8u> CInpainter::makeMask(const CIPPImage<Ipp8u> &image, const CVector3<int> &color)
{
	CIPPImage<Ipp8u> mask(image._size, 1, INPAINTER_SOURCE);
	for(int i=0; i < image._size.width * image._size.height; i++)
		if(		image._imageData[i*3	] == color[0] 
			&&	image._imageData[i*3+1	] == color[1]
			&&	image._imageData[i*3+2	] == color[2])
				mask._imageData[i] = INPAINTER_TARGET;

	return mask;
}

inline void CInpainter::makeMaskedImage(void)
{
	assert(_isValid);

	IppiPoint origin = {0, 0};
	// create inverted mask
	_maskInverted8u = _mask8u.invertBinaryMask();
	// copy region masked
	CIPPImage<Ipp8u> imageMasked8u(_size, 3, Ipp8u(0));
	imageMasked8u.copyRegionMasked(origin, _image8u, origin, _size, _maskInverted8u, origin);
	_imageMasked32f = convert_8u32f(imageMasked8u);
}

// =====================================================================
// The following functions involve double loops and are unlikely to be
// made inline by the compilers. However, I trust the compilers and will
// let it make the decision
// ---------------------------------------------------------------------
inline void CInpainter::setPriorityMap(IppiPoint start, IppiSize roiSize)
{
	assert(_isValid
		&& start.x >= 0 && start.y >= 0 
		&& start.x + roiSize.width <=_size.width
		&& start.y + roiSize.height <=_size.height);

	for(int y = start.y; y < start.y+roiSize.height ; y++)
	for(int x = start.x; x < start.x+roiSize.width; x++)
		if(_mask8u.getPixel(x, y) == INPAINTER_BOUNDARY)
			_priorityMap.setPixel(x, y, getPriority(x, y));
}

inline void CInpainter::findMaxPriority(int &xp, int &yp)
{
	// locate the position with max priority
	float priorityMax = 0;
	for(int  y = 0; y < _size.height; y++)
	{	for(int x = 0; x <_size.width; x++)
		{
			// find the boundary pixel with highest priority
			float priority = _priorityMap.getPixel(x, y);
			if(_mask8u.getPixel(x, y) == INPAINTER_BOUNDARY
				&& priority > priorityMax)
				xp = x, yp = y, priorityMax = priority;
		}
	}
}

inline void CInpainter::setBoundary(IppiPoint start, IppiSize roiSize)
{
	int height =_size.height;
	int width = _size.width;
	for(int y= start.y; y < start.y+roiSize.height; y++)
	    for(int x = start.x; x< start.x+roiSize.width; x++)
		{
			long p = y*width+ x;
			if(INPAINTER_TARGET == _mask8u._imageData[p])
				//if one of the four neighbours is a source pixel, then this is a boundary pixel
				if(y == height-1 || y == 0 || x == 0 || x == width-1
					||_mask8u._imageData[ p+width	]==INPAINTER_SOURCE
					||_mask8u._imageData[ p-width	]==INPAINTER_SOURCE
					||_mask8u._imageData[ p-1		]==INPAINTER_SOURCE
					||_mask8u._imageData[ p+1		]==INPAINTER_SOURCE)
					  _mask8u._imageData[ p         ]= INPAINTER_BOUNDARY;
		}
}


#endif // ___INPAINTER__H___
