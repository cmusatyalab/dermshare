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
 * @file IPPImage.h
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
#ifndef __IPP_IMAGE_H__
#define __IPP_IMAGE_H__

// warning C4312: 'type cast' : conversion from 'CvSubdiv2DEdge' to 'CvQuadEdge2D *' of greater size
//#pragma warning ( disable: 4312 )


//#define _CRTDBG_MAP_ALLOC 
//#include <stdlib.h>
//#include <crtdbg.h>


//#include<vld.h>
//#define VLD_AGGREGATE_DUPLICATES
//#define VLD_MAX_DATA_DUMP 16


#include "ipp.h"
#include "ippcv.h"

#include "cv.h"
#include "highgui.h"

#include "diamond/rgb.h"

// IPP version
#ifndef IPP_3_0
#define IPP_3_0 0
#endif

#ifndef IPP_5_2
#define IPP_5_2 1
#endif


#include <iostream>
#include <cassert>
#include <cfloat>
#include <cmath>
#include <vector>


#ifndef PI
#define PI 3.1415926
#endif

#ifndef ROUND
#define ROUND(x) int(floor(x+0.5))
#endif

// DIR: Direction
enum{
	IPPIMAGE_HORIZONTAL = 0,
	IPPIMAGE_VERTICAL = 1
};

// BBT: Bounding Box Type
enum{
	IPPIMAGE_BBT_VALID = 0,	// result as big as the process will produce
	IPPIMAGE_BBT_SAME = 1	// result same size as input
};

//typedef _ippiBoundingBoxType
//{
//	IPPI_BBT_VALID;	// result as big as the process will produce
//	IPPI_BBT_SAME;	// result same size as input
//} IppiBoundingBoxType;

/// @namespace using standard namespace
using namespace std;


	
// ==================================================================================
/**
 * @class CIPPImage
 * @brief Basic image class that serves as an interface
 * for Intel's IPP library\n
 * functions connects ipp with OpenCV's highgui
 *
 * @ingroup terrain
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
 * @date 2004-12-31
 * @author howardz
 *
 * @todo Implement det() and invert()
 * @bug none
 * @warning none
 */
//================================================================================

template<class T>
class CIPPImage
{
public:
	T * _imageData;				///< pointer to the image data
	IppiSize _size;				///< size of the image
	int _stepBytes;				///< step is the number of bytes between each scan line of the image
	int _nChannels;				///< number of channels (1, 3, 4)

private:
	// ------------------------------------------------------
	// private helpers
	// ------------------------------------------------------

public:
	// ------------------------------------------------------
	// constructors and destructors
	// ------------------------------------------------------
	/// Default constructor. Creates an uninitialized ipp image.
	CIPPImage(void){_imageData = NULL;};

	CIPPImage(IppiSize size, int nChannels);
	CIPPImage(IppiSize size, int nChannels, const T value);
	CIPPImage(int width, int height, int nChannels);
	CIPPImage(int width, int height, int nChannels, const T value);
	CIPPImage(int widht, int height, int nChannels, const T* data);

	/// copy constructor
	CIPPImage(const CIPPImage<T> &refImage);

	/// destructor. deallocate the image memory if necessary
	virtual ~CIPPImage(void)
	{	if(NULL != _imageData)
		{	ippiFree(_imageData);
			_imageData = NULL;
		}
	}

	// ------------------------------------------------------
	// Mutator and Accessor
	// ------------------------------------------------------
	int width(void)	const	{return _size.width;}	
	int height(void)const	{return _size.height;}	
	T*  data(void)	const	{return _imageData;}	

	// ------------------------------------------------------
	// Predicators
	// ------------------------------------------------------
	/**
	 * Check whether the image block is initialized
	 * @return true if is initialized or otherwise
	 */
	bool isInitialized(void) const
	{ return (_imageData != NULL);}

	// suppose some part of this image is masked with the maskvalue
	// we want to get the minimum rectangular region that contains all the unmasked contents 
	bool getUnmaskedRect(T maskValue, int &xMin, int &xMax, int &yMin, int &yMax) const;
	bool getUnmaskedRect(T maskValue, IppiPoint& start, IppiSize& size) const;

	/**
	 * Check whether the given ROI (Region Of Interest) can
	 * be extracted from the image. 
	 *
	 * @param start starting point of the ROI
	 * @param sizeROI size of the ROI
	 * @return ture if the ROI is valid or otherwise
	 */
	bool isValidRegion(IppiPoint start, IppiSize sizeROI) const
	{
		return (start.x >= 0 &&
				start.y >= 0 &&
				start.x + sizeROI.width <= _size.width &&
				start.y + sizeROI.height <= _size.height);
	}

	// get the lib version
	static void getLibInfo(void) 
	{
		const IppLibraryVersion* lib = ippiGetLibVersion();
		std::cout << lib->Name << " " << lib->Version << " " << lib->major << "." << lib->minor << "." << lib->majorBuild << "." << lib->build;
	}


	// ------------------------------------------------------
	// 4. Image Data Exchange and Initialization Functions
	// ------------------------------------------------------
	/**
	 * Get the pixel intensity value from the given location
	 * currently only meaningful for single channel image
	 *
	 * @param x,y coordinates
	 * @return pixel intensity value 
	 */
	T getPixel(const int x, const int y) const
	{
		assert(x >= 0 && y >= 0 && x < _size.width && y < _size.height);
		return _imageData[x + y * _size.width];
	}

	T getPixel(const double x, const double y) const;

	/**
	 * Set the intensity value of the given pixel location
	 *
	 * @param x,y coordinates
	 * @param value the desired intensity value
	 */
	void setPixel(const int x, const int y, const T value)
	{
		assert(x >= 0 && y >= 0 && x < _size.width && y < _size.height);
		_imageData[x+y*_size.width] = value;
	}
 
	CIPPImage<T> getRegion(IppiPoint start, IppiSize sizeROI) const;
	CIPPImage<T> getRegion(int x0, int y0, int x1, int y1) const
	{
		IppiPoint start = {x0, y0};
		IppiSize size = {x1 - x0 + 1, y1 - y0 + 1};
		return getRegion(start, size);
	}

	CIPPImage<T> getChannel(int channel) const;

	CIPPImage<T> getRegionCenter(IppiSize sizeROI) const;

	IppStatus set(const T value);
	IppStatus set(const T value, const CIPPImage<Ipp8u> &mask);
	IppStatus setRegion(const T value, IppiPoint start, IppiSize sizeROI);
	IppStatus setRegionMasked(const T value, IppiPoint start, IppiSize sizeROI, 
						const CIPPImage<Ipp8u> &mask, IppiPoint maskStart); 

	IppStatus copyRegion(IppiPoint start, const CIPPImage<T> &patch, IppiPoint patchStart, IppiSize sizeROI);
	IppStatus copyRegionMask(IppiPoint start, const CIPPImage<T> &patch, IppiPoint patchStart, 
						IppiSize sizeROI, const CIPPImage<T> &mask);
	IppStatus copyRegionMasked(IppiPoint start, const CIPPImage<T> &patch, IppiPoint patchStart, 
						IppiSize sizeROI, const CIPPImage<T> &mask, IppiPoint maskStart);

	IppStatus copyConstBorder(IppiPoint start, IppiSize sizeROI, 
						const CIPPImage<T> &src, IppiPoint srcStart, IppiSize sizeSrcROI, 
						int topBorderWidth, int leftBorderWidth, T value);
	IppStatus copyReplicateBorder(IppiPoint start, IppiSize sizeROI, 
						const CIPPImage<T> &src, IppiPoint srcStart, IppiSize sizeSrcROI, 
						int topBorderWidth, int leftBorderWidth);

	//IppStatus copyRegionMasked(IppiPoint start, const CIPPImage<Ipp16s> &patch, IppiPoint patchStart, 
	//									   IppiSize sizeROI, const CIPPImage<Ipp8u> &mask);

	// ------------------------------------------------------
	// 6. Image Color Conversion
	// ------------------------------------------------------
	CIPPImage<T> RGBToLUV() const;
	CIPPImage<T> LUVToRGB() const;
	CIPPImage<T> BGRToLab() const;
	CIPPImage<T> LabToBGR() const;
	CIPPImage<T> RGBToHSV() const;
	CIPPImage<T> RGBToBGR() const;
	CIPPImage<T> BGRToRGB() const;

	// Converts an RGB image to gray scale using fixed transform coefficients.
	CIPPImage<T> RGBToGray() const;
	CIPPImage<T> grayToRGB() const;

	// ------------------------------------------------------
	// 8. Morphological Operation
	// ------------------------------------------------------
	CIPPImage<T> morphDilate3x3		(void)			const;
	CIPPImage<T> morphErode3x3		(void)			const;

	CIPPImage<T> morphDilate		(const Ipp8u *pMask, IppiSize maskSize, IppiPoint anchor)	const;
	CIPPImage<T> morphErode			(const Ipp8u *pMask, IppiSize maskSize, IppiPoint anchor)	const;

	CIPPImage<T> morphDilateBorder	(const Ipp8u *pMask, IppiSize maskSize, IppiPoint anchor)	const;
	CIPPImage<T> morphErodeBorder	(const Ipp8u *pMask, IppiSize maskSize, IppiPoint anchor)	const;
	CIPPImage<T> morphCloseBorder	(const Ipp8u *pMask, IppiSize maskSize, IppiPoint anchor)	const;
	CIPPImage<T> morphOpenBorder	(const Ipp8u *pMask, IppiSize maskSize, IppiPoint anchor)	const;
	CIPPImage<T> morphTophatBorder	(const Ipp8u *pMask, IppiSize maskSize, IppiPoint anchor)	const;
	CIPPImage<T> morphBlackhatBorder(const Ipp8u *pMask, IppiSize maskSize, IppiPoint anchor)	const;
	CIPPImage<T> morphGradientBorder(const Ipp8u *pMask, IppiSize maskSize, IppiPoint anchor)	const;


	// ------------------------------------------------------
	// 9. Filtering Functions
	// ------------------------------------------------------
	CIPPImage<T> getGradient			(int direction) const;
	CIPPImage<T> getGradientSameSize	(int direction) const;
	CIPPImage<T> getGradientMagnitude	(void)			const;
	CIPPImage<T> getDivergence			(void)			const;

	// The IppiMaskSize enumeration defines the neighborhood area for some
	//	morphological and filtering functions:
	//	typedef enum {
	//		ippMskSize1x3 = 13,
	//		ippMskSize1x5 = 15,
	//		ippMskSize3x1 = 31,
	//		ippMskSize3x3 = 33,
	//		ippMskSize5x1 = 51,
	//		ippMskSize5x5 = 55
	//	}IppiMaskSize;
	CIPPImage<T> filterGauss	(IppiMaskSize mask = ippMskSize3x3) const;
	CIPPImage<T> filterLaplace	(IppiMaskSize mask = ippMskSize3x3) const;
	CIPPImage<T> filterMedian	(IppiMaskSize mask = ippMskSize3x3) const;

	/**
	 * @param kernelSize Specifies the size of the Gaussian kernel, must be odd and greater
	 *        than or equal to 3.
	 * @param borderType Type of border (see Borders); following values are possible:
	 *        ippBorderConst Values of all border pixels are set to constant.
	 *	      ippBorderRepl Replicated border is used.
	 *	      ippBorderWrap Wrapped border is used
	 *	      ippBorderMirror Mirrored border is used
	 *        ippBorderMirrorR Mirrored border with replication is used
	 **/
	CIPPImage<T> filterGaussBorder(int kernelSize, T sigma, 
		IppiBorderType borderType = ippBorderRepl, T borderValue = 0) const;

	// ------------------------------------------------------
	// 11. Image Statistics Functions
	// ------------------------------------------------------
	double sum() const;
	double sum(IppiPoint start, IppiSize sizeROI) const;

	T getMax(int *pIndexX, int *pIndexY) const;
	T getMin(int *pIndexX, int *pIndexY) const;
	T getMin(int *pIndexX, int *pIndexY, const CIPPImage<Ipp8u>& mask) const;

	int countInRange(IppiPoint start, IppiSize sizeRegion, T lowerBound, T upperBound) const;
	int countInRange(T lowerBound, T upperBound) const;


	// Finds coordinates of peaks (maximums or minimums) with absolute value exceeding threshold value.
	// norm Specifies type of the norm to form the mask for extremum search:
	//		ippiNormInf Infinity norm (8-connectivity, 3x3 rectangular mask);
	//		ippiNormL1 L1 norm (4-connectivity, 3x3 cross mask).
	CIPPImage<Ipp8u> findPeaks3x3(T threshold, vector<IppiPoint> &peaks, int maxPeakCount, IppiNorm norm = ippiNormInf, int border=1) const;
	
	// ------------------------------------------------------
	// 12. Image Geometric Transform
	// ------------------------------------------------------
	CIPPImage<T> mirror(IppiAxis flip = ippAxsHorizontal) const;

	CIPPImage<T> resize(double xFactor, double yFactor, int interpolation = IPPI_INTER_CUBIC) const;
	CIPPImage<T> remap(const CIPPImage<Ipp32f> &xMap, const CIPPImage<Ipp32f> &yMap,
						IppiSize resultSize, int interpolation = IPPI_INTER_CUBIC) const;

	CIPPImage<T> shift(int xShift, int yShift) const;
	CIPPImage<T> shift(double xShift, double yShift, int interpolation = IPPI_INTER_CUBIC) const;

	CIPPImage<T> getRotatedSquarePatch(IppiPoint rotationAxis, double angleInDegree, int radius) const;

	// rotating image counter-clockwise
	CIPPImage<T> rotateAroundCenter(double angle, int interpolation = IPPI_INTER_CUBIC, int boundingBoxType = IPPIMAGE_BBT_VALID) const;
	CIPPImage<T> rotateAroundCenter(double angle, double * pXShift, double * pYShift, int interpolation = IPPI_INTER_CUBIC) const;
	CIPPImage<T> rotateCenter(double angle, double xCenter, double yCenter, int interpolation = IPPI_INTER_CUBIC) const;

	CIPPImage<T> invert() const;

	// treat each image as a binary mask (0 indicates 0, anyother value = 1)
	CIPPImage<Ipp8u> invertBinaryMask() const;

	// make an image into a mask with only two possible values 0 and 1
	CIPPImage<Ipp8u> makeMask(T threshold = 0) const;
	CIPPImage<Ipp8u> makeBinary(T threshold = 0) const { return makeMask(threshold); };

	CIPPImage<T> multiplyCenter (const CIPPImage<T> &mask, IppiPoint center);

	// calculate the bounding rectangle of all nonzero pixels
	void getBoundingRectangle(IppiPoint &start, IppiSize &sizeROI, T threshold) const;

	// ------------------------------------------------------
	// 14. Computer Vision
	// ------------------------------------------------------
	CIPPImage<T> labelMarkers(int &numLabels, int minLabel = 1, int maxLabel = 254, IppiNorm norm = ippiNormInf) const;

	CIPPImage<T> removeSmallRegions(int threshold) const;
	// ------------------------------------------------------
	// Overloaded Operators 
	// ------------------------------------------------------
	// assignment operatore
	CIPPImage<T>& operator= (const CIPPImage<T> &refImage);
	CIPPImage<T>  operator- (void)							const; 
	CIPPImage<T>  operator+ (const CIPPImage<T> &refImage)	const;
	CIPPImage<T>  operator- (const CIPPImage<T> &refImage)	const;
	CIPPImage<T>  operator* (const CIPPImage<T> &mask)		const;
	CIPPImage<T>  operator* (T value)						const;
	CIPPImage<T>  operator/ (double value)					const;
	CIPPImage<T>  operator| (const CIPPImage<T> &refImage) const;
	CIPPImage<T>  operator& (const CIPPImage<T> &refImage) const;

	// ------------------------------------------------------
	// Output or Test
	// ------------------------------------------------------
	IppStatus load				(const char * filename, int fileType = 1, int isColor = -1);
	IppStatus load				(const void *data, size_t len, int isColor = -1);
	IppStatus load				(const RGBImage &image);
	IppStatus loadGray			(const char * filename, int fileType = 1);
	IppStatus loadBinary		(const char * filename, T threshold, int fileType = 1);
	IppStatus save				(const char * filename, int fileType = 1) const;
	IppStatus save				(const char *extension, vector<uchar> &buf) const;
	IppStatus saveScaled		(const char * filename, int fileType = 1) const;

	// ------------------------------------------------------
	// drawing functions
	// ------------------------------------------------------
	void drawPoint		(IppiPoint point,							T color);
	void drawPoint		(int x,	int y, T color);

	void drawLine		(IppiPoint p1, IppiPoint p2,				T color);
	void drawLine		(int x1, int y1, int x2, int y2,			T color);
	void drawLine		(int xc, int yc, float angleInDegree, int length, T color);

	void circlePoints	(int x, int y, int x0, int y0,				T color);
	void drawCircle		(IppiPoint center,	int radius,				T color, bool bFill = false);
	void drawCircle		(int x0, int y0,	int radius,				T color, bool bFill = false);

	void drawBox		(IppiPoint upperLeft, IppiPoint lowerRight, T color, bool bFill = false);
	void drawBox		(IppiPoint upperLeft, IppiSize size,		T color, bool bFill = false);
	void drawBox		(int x1, int y1, int x2, int y2,			T color, bool bFill = false);
	
	void drawSquare		(int x0, int y0,	int size,				T color, bool bFill = false);
	void drawSquare		(IppiPoint center,	int size,				T color, bool bFill = false);

	void drawCross		(int x0, int y0,	int size,				T color);
	void drawCross		(IppiPoint center,	int size,				T color);


	// ------------------------------------------------------
	// Friend functions
	// ------------------------------------------------------


};


// ------------------------------------------------------
// input, output or test
// ------------------------------------------------------
void displayStatus(IppStatus status);

// ========================================================
// gradients and divergence
// --------------------------------------------------------
CIPPImage<Ipp16s>	getGradient			(const CIPPImage<Ipp8u> &image, int direction);
CIPPImage<Ipp16s>	getGradientSameSize	(const CIPPImage<Ipp8u> &image, int direction);
CIPPImage<Ipp16s>	getGradientMagnitude(const CIPPImage<Ipp8u> &image);
CIPPImage<Ipp16s>	getDivergence		(const CIPPImage<Ipp8u> &image);
CIPPImage<Ipp16s>	getDivergence		(const CIPPImage<Ipp16s> & Dx, const CIPPImage<Ipp16s> & Dy);

// the following is for Ipp16u data
CIPPImage<Ipp32f>	getGradient			(const CIPPImage<Ipp16u> &image, int direction);
CIPPImage<Ipp32f>	getGradientSameSize	(const CIPPImage<Ipp16u> &image, int direction);
CIPPImage<Ipp32f>	getGradientMagnitude(const CIPPImage<Ipp16u> &image);
CIPPImage<Ipp32f>	getDivergence		(const CIPPImage<Ipp16u> &image);
CIPPImage<Ipp32f>	getDivergence		(const CIPPImage<Ipp32f> & Dx, const CIPPImage<Ipp32f> & Dy);

// ------------------------------------------------------
// Overloaded Operators 
// ------------------------------------------------------
template<typename T> CIPPImage<T> operator+ (const CIPPImage<T> &image, T value);
template<typename T> CIPPImage<T> operator- (const CIPPImage<T> &image, T value);
//template<typename T> CIPPImage<T> operator* (const CIPPImage<T> &image, T value);
template<typename T> CIPPImage<T> operator+ (T value, const CIPPImage<T> &image);
template<typename T> CIPPImage<T> operator- (T value, const CIPPImage<T> &image);
//template<typename T> CIPPImage<T> operator* (T value, const CIPPImage<T> &image);
template<typename T> CIPPImage<T> operator* (const CIPPImage<T> &image1, const CIPPImage<T> &image2);


// ========================================================
// image statistics
// --------------------------------------------------------
template<typename T>
CIPPImage<Ipp32f>	crossCorrelation						(const CIPPImage<T> &src, const CIPPImage<T> &tpl);
template<typename T>
CIPPImage<Ipp32f>	squaredDifferenceMasked_FFT				(const CIPPImage<T> &src, const CIPPImage<T> &tpl, const CIPPImage<Ipp8u> &mask);
template<typename T>
CIPPImage<Ipp32f>	squaredDifferenceMasked_FFT_C1R			(const CIPPImage<T> &src, const CIPPImage<T> &tpl, const CIPPImage<Ipp32f> &mask);


// ========================================================
// 14. Computer Vision
// --------------------------------------------------------
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
enum{
	DT_CHESSBOARD = ippiNormInf,
	DT_CITYBLOCK = ippiNormL1,
	DT_EUCLIDEAN = ippiNormL2,
	DT_GAUSSIAN_EUCLIDEAN
};
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
 *                ippiNormInf(0)  "chessbord"   delta = max(|x1-x2|, |y1-y2|)
 *                ippiNormL1(1)   "cityblock"   delta = |x1-x2| + |y1-y2|
 *                ippiNormL2(2)   "euclidean"   delta = sqrt((x1-x2)^2 + (y1-y2)^2)
 * @return CIPPImage<Ipp32f>
 *
 **/
template<typename T>
CIPPImage<Ipp32f> distanceTransform(const CIPPImage<T> &src, IppiNorm norm = ippiNormL2, int kerSize = 5);

template<typename T>
CIPPImage<Ipp32f> distanceTransformGeneral(const CIPPImage<T> &src, int DTType = DT_EUCLIDEAN, int kerSize = 5, float sigma = 1);

template<typename T>
CIPPImage<Ipp32f> distanceTransformGuassianEuclidean(const CIPPImage<T> &src, float sigma = 1, int kerSize = 5);


// ========================================================
// image geometric transforms
// --------------------------------------------------------
template<typename T>
CIPPImage<T>	remap(const CIPPImage<T> &source, 
					const CIPPImage<Ipp32f> &xMap, const CIPPImage<Ipp32f> &yMap,
					IppiSize resultSize, int interpolation = IPPI_INTER_CUBIC);


template<typename T>
CIPPImage<T> alphaComp(const CIPPImage<T> &A, const CIPPImage<T> &B, IppiAlphaType alphaType=ippAlphaOver);




// ========================================================
// convesion
// --------------------------------------------------------
CIPPImage<Ipp16s>	convert_8u16s	(const CIPPImage<Ipp8u>		&image);
CIPPImage<Ipp16u>   convert_8u16u	(const CIPPImage<Ipp8u>		&image);
CIPPImage<Ipp32f>	convert_8u32f	(const CIPPImage<Ipp8u>		&image);

CIPPImage<Ipp32f>	convert_16s32f	(const CIPPImage<Ipp16s>	&image);
CIPPImage<Ipp32f>	convert_16u32f	(const CIPPImage<Ipp16u>	&image);

CIPPImage<Ipp8u>	convert_16u8u	(const CIPPImage<Ipp16u>	&image);	
CIPPImage<Ipp8u>	convert_16s8u	(const CIPPImage<Ipp16s>	&image);

CIPPImage<Ipp8u>	convert_32f8u	(const CIPPImage<Ipp32f>	&image, IppRoundMode roundMode=ippRndNear);
CIPPImage<Ipp16s>	convert_32f16s	(const CIPPImage<Ipp32f>	&image, IppRoundMode roundMode=ippRndNear);
CIPPImage<Ipp16u>	convert_32f16u	(const CIPPImage<Ipp32f>	&image, IppRoundMode roundMode=ippRndNear);

CIPPImage<Ipp8u>	scale_32f8u		(const CIPPImage<Ipp32f>	&image, IppRoundMode roundMode=ippRndNear);

//	roundMode Rounding mode which can be ippRndZero or ippRndNear:
//	ippRndZero Specifies that floating-point values must be truncated toward zero.
//	ippRndNear Specifies that floating-point values must be rounded to the nearest integer.


// ========================================================
// fast convesion
// using reference to pass object instead of returning
// but needs to set up memory for the result outside the
// function call
// --------------------------------------------------------
void convert_8u16s	(const CIPPImage<Ipp8u>		&src, CIPPImage<Ipp16s>	&dst);
void convert_8u16u	(const CIPPImage<Ipp8u>		&src, CIPPImage<Ipp16u>	&dst);
void convert_8u32f	(const CIPPImage<Ipp8u>		&src, CIPPImage<Ipp32f>	&dst);

void convert_16s32f	(const CIPPImage<Ipp16s>	&src, CIPPImage<Ipp32f>	&dst);
void convert_16u32f	(const CIPPImage<Ipp16u>	&src, CIPPImage<Ipp32f>	&dst);

void convert_16u8u	(const CIPPImage<Ipp16u>	&src, CIPPImage<Ipp8u>	&dst);	
void convert_16s8u	(const CIPPImage<Ipp16s>	&src, CIPPImage<Ipp8u>	&dst);

void convert_32f8u	(const CIPPImage<Ipp32f>	&src, CIPPImage<Ipp8u>	&dst, IppRoundMode roundMode=ippRndNear);
void convert_32f16s	(const CIPPImage<Ipp32f>	&src, CIPPImage<Ipp16s>	&dst, IppRoundMode roundMode=ippRndNear);
void convert_32f16u	(const CIPPImage<Ipp32f>	&src, CIPPImage<Ipp16u>	&dst, IppRoundMode roundMode=ippRndNear);


template<typename T>
CIPPImage<T> CIPPImage<T>::getRegionCenter(IppiSize sizeROI) const
{
	assert(_size.width >= sizeROI.width && _size.height >= sizeROI.height);

	IppiPoint start;
	start.x = (_size.width - sizeROI.width)/2;
	start.y = (_size.height - sizeROI.height)/2;

	return getRegion(start, sizeROI);
}

template<typename T>
T CIPPImage<T>::getPixel(const double xin, const double yin) const
{
	T value;
	float x = float(xin);
	float y = float(yin);
	float x1 = floor(x);
	float y1 = floor(y);

	if(x == x1 && y == y1)
	{
		return _imageData[int(x1) + int(y1) *_size.width];
	}
	else if(x == x1)
	{
		float y2 = ceil(y);
		return (_imageData[int(x1) + int(y1) *_size.width]*(y2-y) + _imageData[int(x1) + int(y2) *_size.width]*(y-y1));
	}
	else if(y == y1)
	{
		float x2 = ceil(x);
		return (_imageData[int(x1) + int(y1) *_size.width]*(x2-x) + _imageData[int(x2) + int(y1) *_size.width]*(x-x1));
	}
	else
	{
		float x2 = ceil(x);
		float y2 = ceil(y);

		//assert(x1 >= 0 && x2 < _size.width && y1 >= 0 && y2 < _size.height);
		
		T lowerLeft		= _imageData[int(x1) + int(y1)*_size.width];
		T lowerRight	= _imageData[int(x2) + int(y1)*_size.width];
		T upperLeft		= _imageData[int(x1) + int(y2)*_size.width];
		T upperRight	= _imageData[int(x2) + int(y2)*_size.width];

		// use bilinear interpolation.
		value = T((y2-y)*(lowerLeft*(x2-x) + lowerRight*(x-x1)) + (y-y1)*(upperLeft*(x2-x) + upperRight*(x-x1)));
	}

	return value;
}

// shift the image
template<typename T>
CIPPImage<T> CIPPImage<T>::shift(double xShift, double yShift, int interpolation) const
{
	CIPPImage<Ipp32f> xMap(_size, _nChannels, 0);
	CIPPImage<Ipp32f> yMap(_size, _nChannels, 0);

	// TODO: using remap to do a simple shift operation is really a waste of time and resource
	// come up with a navie shift algorithm
	double xValue, yValue;

	// make the xMap and yMap
	for(int x = 0; x < _size.width; x++)
	{
		for(int y = 0; y < _size.height; y++)
		{
			xValue = x - xShift;
			yValue = y - yShift;
			
			xMap.setPixel(x, y, (Ipp32f)xValue);
			yMap.setPixel(x, y, (Ipp32f)yValue);
		}
	}

	return remap(xMap, yMap, _size);
}

template<typename T>
CIPPImage<T> CIPPImage<T>::shift(int xShift, int yShift) const
{
	IppiPoint startSrc = {0, 0};
	IppiPoint startDst = {0, 0};
	IppiSize sizeROI = {_size.width - abs(xShift), _size.height - abs(yShift)};

	if(xShift < 0)
		startSrc.x = -xShift;
	else
		startDst.x = xShift;

	if(yShift < 0)
		startSrc.y = -yShift;
	else
		startDst.y = yShift;

	CIPPImage<T> result(_size, _nChannels, 0);
	result.copyRegion(startDst, *this, startSrc, sizeROI);
	return result;
}



template<typename T>
inline void CIPPImage<T>::drawPoint(IppiPoint point, T color)
{
	setPixel(point.x, point.y, color);
}

template<typename T>
inline void CIPPImage<T>::drawPoint(int x, int y, T color)
{
	setPixel(x, y, color);
}


template<typename T>
void CIPPImage<T>::drawLine(int x1, int y1, int x2, int y2, T color)
{
	assert(x1 >= 0 && x1 <= _size.width-1 && 
		x2 >= 0 && x2 <= _size.width-1 && 
		y1 >= 0 && y1 <= _size.height-1 && 		
		y2 >= 0 && y2 <= _size.height-1);

	float xFloat, yFloat;
	int x, y;
	int dx = x2 - x1;
	int dy = y2 - y1;
	int length;
	float xinc, yinc;
	int xincInt, yincInt;

	// determine whether horizontal or vertical change is larger
	if (abs(dx) > abs(dy))
		length = abs(dx);
	else
		length = abs(dy);


	// special case to avoid dividing by zero
	if (length == 0) {
		setPixel(x1, y1, color);
		return;
	}

	xinc = float(dx) / length;
	yinc = float(dy) / length;
	x = x1;
	y = y1;
	// -------------------------------------------
	// special cases
	// -------------------------------------------
	// horizontal
	if (0 == dy)
	{
		xincInt = int(xinc);
		for (int i = 0; i <= length; i++) 
		{
			setPixel (x, y, color);
			x += xincInt;
		}
		return;
	}
	// vertical
	if (0 == dx)
	{
		yincInt = int(yinc);
		for (int i = 0; i <= length; i++) 
		{
			setPixel (x, y, color);
			y += yincInt;
		}
		return;
	}

	float m = float(dy)/float(dx);
	// slope m = 1 or -1, 45 degree upwards
	if(1 == m || -1 == m)
	{
		xincInt = int(xinc);
		yincInt = int(yinc);
		for (int i = 0; i <= length; i++) 
		{
			setPixel (x, y, color);
			x += xincInt;
			y += yincInt;
		}
		return;
	}

	xFloat = float(x);
	yFloat = float(y);
	// write "length" number of pixels along the line
	for (int i = 0; i <= length; i++)
	{
		setPixel(int(floor(xFloat+0.5)), int(floor(yFloat+0.5)), color);
		xFloat += xinc;
		yFloat += yinc;
	}
}

template<typename T>
inline void CIPPImage<T>::drawLine(int xc, int yc, float angleInDegree, int length, T color)
{
	assert(length > 0 && color > 0);
	assert(xc >= 0 && xc <= _size.width-1 && 
		yc >= 0 && yc <= _size.height-1);

	float lengthHalf = length/2.f;
	float angleInRadian = angleInDegree * PI/180;
	// get the starting line and ending line
	int xh = int(lengthHalf * cos(angleInRadian));
	int yh = int(lengthHalf * sin(angleInRadian));
	int x1 = min(max(xc - xh, 0), _size.width-1);
	int y1 = min(max(yc - yh, 0), _size.height-1);
	int x2 = min(max(xc + xh, 0), _size.width-1);
	int y2 = min(max(yc + yh, 0), _size.height-1);
	drawLine(x1, y1, x2, y2, color);
}


template<typename T>
inline void CIPPImage<T>::drawLine(IppiPoint p1, IppiPoint p2, T color)
{
	drawLine(p1.x, p1.y, p2.x, p2.y, color);
}


template<typename T>
inline void CIPPImage<T>::drawBox(int x1, int y1, int x2, int y2, T color, bool bFill)
{
	assert(x2 >= x1 && y2 >= y1);
	assert(x1 >= 0 && x2 <= _size.width-1 && y1 >= 0 && y2 <= _size.height-1);

	if(bFill)
	{
		for(int y = y1; y <= y2; y++)
		{
			for(int x = x1; x <= x2; x++)
			{
				setPixel(x, y, color);
			}
		}
	}
	else
	{
		// just draw the 4 lines
		for(int x = x1; x <= x2; x++)
		{
			setPixel(x, y1, color);
			setPixel(x, y2, color);
		}
		for(int y = y1+1; y <= y2-1; y++)
		{
			setPixel(x1, y, color);
			setPixel(x2, y, color);
		}
	}
}

template<typename T>
inline void CIPPImage<T>::drawBox(IppiPoint upperLeft, IppiPoint lowerRight,  T color, bool bFill)
{
	drawBox(upperLeft.x, upperLeft.y, lowerRight.x, lowerRight.y, color, bFill);
}

template<typename T>
inline void CIPPImage<T>::drawBox(IppiPoint upperLeft, IppiSize size, T color, bool bFill)
{
	IppiPoint lowerRight;
	lowerRight.x = upperLeft.x + size.width - 1;
	lowerRight.y = upperLeft.y + size.height - 1;
	drawBox(upperLeft, lowerRight, color, bFill);
}

template<typename T>
inline void CIPPImage<T>::drawSquare(int x0, int y0, int size, T color, bool bFill)
{	
	IppiPoint upperLeft;
	int halfSize = int(float(size)/2);
	upperLeft.x = x0 - halfSize;
	upperLeft.y = y0 - halfSize;
	IppiSize boxSize;
	boxSize.height = size;
	boxSize.width = size;
	drawBox(upperLeft, boxSize, color, bFill);	
}

template<typename T>
inline void CIPPImage<T>::drawSquare(IppiPoint center, int size, T color, bool bFill)
{
	drawSquare(center.x, center.y, size, color, bFill);
}

template<typename T>
void CIPPImage<T>::drawCross(int x0, int y0, int size, T color)
{
	int halfSize = int(float(size)/2);
	int x = x0 - halfSize;
	int y = y0 - halfSize;

	assert(x >= 0 && _size.width >= x + size + 1 &&
		y >= 0 && _size.height >= y + size + 1);

	for(int i = 0; i <= size; i++)
	{
		setPixel(x, y0, color);
		x++;
	}
	for(int i = 0; i <= size; i++)
	{
		setPixel(x0, y, color);
		y++;
	}
}

template<typename T>
void CIPPImage<T>::drawCross(IppiPoint center, int size, T color)
{
	drawCross(center.x, center.y, size, color);
}


// use of eight-way symmetry
template<typename T>
inline void CIPPImage<T>::circlePoints(int x, int y, int x0, int y0, T color)
{
	setPixel(x+x0, y+y0, color);
	setPixel(y+x0, x+y0, color);
	setPixel(y+x0, -x+y0, color);
	setPixel(x+x0, -y+y0, color);
	setPixel(-x+x0, -y+y0, color);
	setPixel(-y+x0, -x+y0, color);
	setPixel(-y+x0, x+y0, color);
	setPixel(-x+x0, y+y0, color);
}



template<typename T>
void CIPPImage<T>::drawCircle(int x0, int y0, int radius, T color, bool bFill)
{
	assert( x0 - radius >= 0 && x0 + radius <= _size.width - 1 &&
		y0 - radius >= 0 && y0 + radius <= _size.height + 1);

	int x = 0;
	int y = radius;
	int d = 1 - radius;
	int deltaE = 3;
	int deltaSE = -2 * radius + 5;
	circlePoints(x, y, x0, y0, color);

	if(!bFill)
	{
		while(y > x)
		{
			// select E
			if(d < 0)
			{
				d += deltaE;
				deltaE += 2;
				deltaSE += 2;
			}
			else
			{
				d += deltaSE;
				deltaE += 2;
				deltaSE += 4;
				y--;
			}
			x++;
			circlePoints(x, y, x0, y0, color);
		}
	}
	else // TODO: a naive to fill the circle
	{
		while(y > x)
		{
			// select E
			if(d < 0)
			{
				d += deltaE;
				deltaE += 2;
				deltaSE += 2;
			}
			else
			{
				// this 8 point algorithm will
				// fill the outter part of the circle
				// where y > x
				// it will leave a square area inside the circle
				// that would be the area we want to fill
				for(int xf = 0; xf < x; xf++)
				{
					circlePoints(xf, y, x0, y0, color);
				}
				d += deltaSE;
				deltaE += 2;
				deltaSE += 4;
				y--;
			}
			x++;
			circlePoints(x, y, x0, y0, color);
		}

		// once out of the loop, here y == x
		// now we fill the square inside
		// a simple calculation can extract the boundary
		int a = y;

		for(int ys = y0 - a; ys <= y0 + a; ys++)
			for(int xs = x0 - a; xs <= x0 + a; xs++)
				setPixel(xs, ys, color);
	}
}


template<typename T>
inline void CIPPImage<T>::drawCircle(IppiPoint center, int radius, T color, bool bFill)
{
	drawCircle(center.x, center.y, radius, color, bFill);
}



// suppose some part of this image is masked with the maskvalue
// we want to get the minimum rectangular region that contains all the unmasked contents 
template<typename T>
bool CIPPImage<T>::getUnmaskedRect(T maskValue, int &xMin, int &xMax, int &yMin, int &yMax) const
{
	xMin = -1;
	yMin = -1;
	xMax = -1;
	yMax = -1;

	// first, find the boundary
	for(int y = 0; y < _size.height; y++)
	{
		for(int x = 0; x < _size.width; x++)
			if(maskValue != getPixel(x, y))
			{	
				yMin = y;
				break;
			}

		if(yMin != -1)
			break;
	}

	if(yMin != -1)
	{
		for(int x = 0; x < _size.width; x++)
		{
			for(int y = yMin; y < _size.height; y++)
				if(maskValue != getPixel(x, y))
				{
					xMin = x;
					break;
				}
		
			if(xMin != -1)
				break;
		}


		for(int y = _size.height-1; y >= yMin; y--)
		{
			for(int x = xMin; x < _size.width-1; x++)
				if(maskValue != getPixel(x, y))
				{
					yMax = y;
					break;
				}

			if(yMax != -1)
				break;
		}

		for(int x = _size.width-1; x >= xMin; x--)
		{
			for(int y = yMin; y <= yMax; y++)
				if(maskValue != getPixel(x, y))
				{
					xMax = x;
					break;
				}
				
			if(xMax != -1)
				break;
		}

		return true;
	}
	else // the entire image is masked with maskvalue
		return false;
}

// suppose some part of this image is masked with the maskvalue
// we want to get the minimum rectangular region that contains all the unmasked contents 
template<typename T>
bool CIPPImage<T>::getUnmaskedRect(T maskValue, IppiPoint& start, IppiSize& size) const
{
	int xMin, xMax, yMin, yMax;
	bool result = getUnmaskedRect(maskValue, xMin, xMax, yMin, yMax);

	if(result)
	{
		start.x = xMin;
		start.y = yMin;
		size.width = xMax - xMin + 1;
		size.height = yMax - yMin + 1;
	}
	else
	{
		start.x = 0;
		start.y = 0;
		size.width = 0;
		size.height = 0;
	}
	return result;
}



/**
 * calculate the bounding rectangle of all nonzero pixels
 *
 * @param
 * @param
 * @return 
 **/
template<typename T>
void CIPPImage<T>::getBoundingRectangle(IppiPoint &start, IppiSize &sizeROI, T threshold) const
{
	assert(isInitialized());
	assert(1 == _nChannels);

	int xMin = _size.width-1; 
	int xMax = 0;
	int yMin = _size.height-1;
	int yMax = 0;

	for(int y = 0; y < _size.height; y++)
		for(int x = 0; x < _size.width; x++)
		{
			if(threshold < _imageData[y*_size.width+x])
			{
				xMin = min(xMin, x);
				xMax = max(xMax, x);
				yMin = min(yMin, y);
				yMax = max(yMax, y);
			}
		}
		
	start.x = xMin;
	start.y = yMin;
	sizeROI.width = xMax - xMin + 1;
	sizeROI.height = yMax - yMin + 1;
}

// rotating image counter-clockwise
template<typename T>
CIPPImage<T> CIPPImage<T>::rotateAroundCenter(double angle, int interpolation, int boundingBoxType) const
{
	double xShift, yShift;
	switch(boundingBoxType)
	{
	case IPPIMAGE_BBT_VALID:
		return rotateAroundCenter(angle, &xShift, &yShift, interpolation);
		break;
	case IPPIMAGE_BBT_SAME:
		return rotateAroundCenter(angle, &xShift, &yShift, interpolation).getRegionCenter(_size);
		break;
	default:
		cout << "\nWarning -- [rotateAroundCenter] invalid bounding box type; using default bounding box type (VALID) instead";
		return rotateAroundCenter(angle, &xShift, &yShift, interpolation);	
	}
}



// ------------------------------------------------------
// input, output or test
// ------------------------------------------------------

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
template<typename T>
inline IppStatus CIPPImage<T>::loadGray(const char * filename, int fileType)
{	// force it to be loaded as gray scale
	return load(filename, fileType, 0);
}


// ------------------------------------------------------
// 11. Image Statistics Functions
// ------------------------------------------------------


template<typename T>
CIPPImage<T> CIPPImage<T>::grayToRGB(void) const
{
	assert(isInitialized());
	if(1 != _nChannels) return (*this);

	CIPPImage<T> result(_size, 3);
	int offset3;
	T value;
	for(int offset = 0; offset < _size.width * _size.height ; offset++)
	{
		offset3 = offset * 3;
		value = _imageData[offset];
		result._imageData[offset3]   = value;
		result._imageData[offset3+1] = value;
		result._imageData[offset3+2] = value;
	}
	return result;
}






#endif // __IPP_IMAGE_H__
