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
 * Segmenter
 * @file segmenter.h
 *
 * Primary Authors: Howard Zhou (howardz@cc.gatech.edu)\n
 *
 *
 * ==================================================================\n
 * @reference
 *
 *
 * @version 1.0\n
 *     Initial Revision
 * @date 2007-10-10
 * @author Howard Zhou
 *
 *
 * @todo
 * @bug
 * @warning 
 *
 * ===================================================================\n
 */

#ifndef ___SEGMENTER__H___
#define ___SEGMENTER__H___

// #pragma warning(disable : 4786)

#include <fstream>
#include <vector>

#include "IPPInterface/IPPImage.h"

#include "math/Vector.h"
#include "DataStructure/Region.h"

#include "diamond/diamond.h"

#include "segmenter/segmenterEnums.h"

#ifndef PI
#define PI 3.1415926
#endif

/// @namespace using standard namespace
using namespace std;

// coordinateToColorRatio is to normalize the coordinate
// values against the color value
// e.g.
// using cartesian coordinate system, we have x,y ~ [0,...,1] and r,g,b ~ [0,...,1]
// however, these are not the same metrics. coordinateToColorRatio let the user
// to pick a ratio that makes spacial dimension and colorspace dimension compatible
#define COORDINATE_TO_COLOR_RATIO_RGB 0.0167	// for RGB
#define COORDINATE_TO_COLOR_RATIO_LAB 0.05		// for LAB
//#define COORDINATE_TO_COLOR_RATIO_LAB 0.0167


// ==================================================================================
/**
 * @class CSegmenter
 * @brief An Unbiased Detector of Curvilinear Structures
 * @ingroup
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
 * @date 2007-10-10
 * @author Howard Zhou
 *
 * 
 * @todo
 * @bug
 * @warning
 */
// ================================================================================

class CSegmenter
{
protected:
	CIPPImage<Ipp8u>	_imageOriginal;
	CIPPImage<Ipp8u>	_image;				// working copy
	CIPPImage<Ipp8u>	_imageInClusters;
	CIPPImage<Ipp8u>	_mask;

	CIPPImage<Ipp8u>	_clusterMap;
	CIPPImage<Ipp8u>	_superClusterMap;

	vector<CRegion<CVector<float> > > _clusters;
	vector<CRegion<CVector<float> > > _superClusters;

	int			_colorSpace;
	int			_coordinateSystem;
	float		_coordinateToColorRatio;

	bool		_bCluster;			// do we first color the colors?
	bool		_bMask;				// only do pixels within the masked area
	//bool			_bBlur;				// do we blur the image?


	/// state flags
	bool		_isValid;
	bool		_isInitialized;
	bool		_isProcessed;

	/// results

	// temp image files
	CIPPImage<Ipp8u> _segmFiltered;
	CIPPImage<Ipp8u> _segmFilled;
	CIPPImage<Ipp8u> _imageInSegs;
public:
	// ------------------------------------------------------
	// constructors and destructors
	// ------------------------------------------------------
	/// default constructor
	CSegmenter()
		:_isValid(false),_isInitialized(false),_isProcessed(false)
	{};
	
	// use member initialization list.
	CSegmenter(const CIPPImage<Ipp8u> &image,
					int colorSpace = COLORSPACE_CIE_LAB, 
					int coordinateSystem = COORDINATESYSTEM_POLAR,
					bool bCluster = true);

	CSegmenter(const CIPPImage<Ipp8u> &image,
					const CIPPImage<Ipp8u> &mask,
					int colorSpace = COLORSPACE_CIE_LAB, 
					int coordinateSystem = COORDINATESYSTEM_POLAR,
					bool bCluster = true);


	/// copy constructor
	// since we don't have dynamically allocated member, copy constructor is not necessary
	//CSegmenter(const CSegmenter<T> & p){}
		
	/// destructor
	~CSegmenter(){};

	/// initializer
	void initialize(void);

	/// finalize the process
	void commit(void);

	void setImage(const CIPPImage<Ipp8u> &image,
				  int colorSpace = COLORSPACE_CIE_LAB, 
				  int coordinateSystem = COORDINATESYSTEM_POLAR,
				  bool bCluster = true);

	void setImage(const CIPPImage<Ipp8u> &image,
				  const CIPPImage<Ipp8u> &mask,
				  int colorSpace = COLORSPACE_CIE_LAB, 
				  int coordinateSystem = COORDINATESYSTEM_POLAR,
				  bool bCluster = true);

	// ------------------------------------------------------
	// Predicators
	// ------------------------------------------------------
	bool isValid		()const { return _isValid;		};
	bool isInitialized	()const { return _isInitialized;};
	bool isProcessed	()const { return _isProcessed;	};

	// ------------------------------------------------------
	// Accessors and Mutators
	// ------------------------------------------------------

	CIPPImage<Ipp8u> getImageOriginal	(void)const { return _imageOriginal;	};
	CIPPImage<Ipp8u> getImage			(void)const	{ return _image;			};
	CIPPImage<Ipp8u> getImageInClusters	(void)const	{ return _imageInClusters;	};

	CIPPImage<Ipp8u> getClusterMap		(void)const { return _clusterMap;		};
	
	vector<CVector3<float> >  getColors (void)const;
	

	// temp image files
	CIPPImage<Ipp8u> getSegmFiltered	(void)const	{ return _segmFiltered;		};
	CIPPImage<Ipp8u> getSegmFilled		(void)const	{ return _segmFilled;		};
	
	// display all the color ellipse
	void displayColors();

	// ------------------------------------------------------
	// Overloaded Operators 
	// ------------------------------------------------------

	// ------------------------------------------------------
	// Input, Output, Test
	// ------------------------------------------------------
	void load(CIPPImage<Ipp8u> image,
			  int colorSpace = COLORSPACE_CIE_LAB,
			  int coordinateSystem = COORDINATESYSTEM_POLAR,
			  bool bCluster = true);

	void load(const char * filename, 
			  int colorSpace = COLORSPACE_CIE_LAB, 
			  int coordinateSystem = COORDINATESYSTEM_POLAR, 
			  bool bCluster = true);

	void load(const char * filename,
		      const char * maskFilename,
			  int colorSpace = COLORSPACE_CIE_LAB, 
			  int coordinateSystem = COORDINATESYSTEM_POLAR, 
			  bool bCluster = true);

	void save(ImageMap &images, CDiamondContext &diamond);
	
	// ------------------------------------------------------
	// Other operation
	// ------------------------------------------------------

	void clusterKMeans(int K, int numIterations);

	void segment(void);
	void regionHistogram(void);

	CIPPImage<Ipp8u> generateSkinMask(		const CIPPImage<Ipp8u> &superClusterMap, 
											vector<CRegion<CVector<float> > > &superClusters) const;
	CIPPImage<Ipp8u> generateSkinMaskStatistical(const CIPPImage<Ipp8u> &superClusterMap, 
											vector<CRegion<CVector<float> > > &superClusters) const;
	CIPPImage<Ipp8u> generateSkinMaskStatistical2(const CIPPImage<Ipp8u> &superClusterMap, 
											vector<CRegion<CVector<float> > > &superClusters) const;

	CIPPImage<Ipp8u> assignClusterLabels(	const CIPPImage<Ipp8u> &image,
											vector<CRegion<CVector<float> > > &clusters, 
											CIPPImage<Ipp8u> &imageInClusters, 
											int coordinateSystem,
											bool bMask = false,
											const CIPPImage<Ipp8u> &mask = CIPPImage<Ipp8u>(0, 0, 1, (uint8_t *) NULL));

	CIPPImage<Ipp8u> assignForegroundLabels(vector<CRegion<CVector<float> > > &superClusters,
											vector<CRegion<CVector<float> > > &clusters,
											const CIPPImage<Ipp8u> &superClusterMap,
											const CIPPImage<Ipp8u> &clusterMap,
											int cooridnateSystem);

	CIPPImage<Ipp8u> assignForegroundLabelsTest(vector<CRegion<CVector<float> > > &superClusters,
											vector<CRegion<CVector<float> > > &clusters,
											const CIPPImage<Ipp8u> &superClusterMap,
											const CIPPImage<Ipp8u> &clusterMap,
											int cooridnateSystem);

	bool isCamera2(const CRegion<CVector<float> > &superClusters);


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

void assignParent(vector<CRegion<CVector<float> > > &children,
				  vector<CRegion<CVector<float> > > &parents);

// original
vector<CVector<float> >samplePixels(const CIPPImage<Ipp8u> &image,
									int numPoints, 
									int cooridnateSystem = COORDINATESYSTEM_NONE, 
									int colorSpace = COLORSPACE_RGB,
									bool bMask = false,
									const CIPPImage<Ipp8u> &mask = CIPPImage<Ipp8u>(0, 0, 1, (uint8_t *) NULL));


CIPPImage<Ipp8u> assignSuperClusterLabels(const CIPPImage<Ipp8u> &clusterMap,
									const vector<CRegion<CVector<float> > > &clusters,
									const vector<CRegion<CVector<float> > > &superClusters,
									CIPPImage<Ipp8u> &imageInSuperClusters);

CIPPImage<Ipp8u> showClusterMap(	const CIPPImage<Ipp8u> &clusterMap,
									const vector<int> &clusterLabels);


CIPPImage<Ipp8u> cleanMask(const CIPPImage<Ipp8u> &mask);

// ------------------------------------------------------
// constructors and destructors
// ------------------------------------------------------
inline 
CSegmenter::CSegmenter(const CIPPImage<Ipp8u> &image, int colorSpace, int coordinateSystem, bool bCluster)
	:_colorSpace(colorSpace),
	_coordinateSystem(coordinateSystem),
	_bCluster(bCluster),
	_bMask(false),
	_isValid(false),
	_isInitialized(false),
	_isProcessed(false)
{
	assert(image.isInitialized());
	assert(3 == image._nChannels);

	_imageOriginal = image;
	_isValid = true;
	initialize();
}

inline 
CSegmenter::CSegmenter(const CIPPImage<Ipp8u> &image, const CIPPImage<Ipp8u> &mask, int colorSpace, int coordinateSystem, bool bCluster)
	:_colorSpace(colorSpace),
	_coordinateSystem(coordinateSystem),
	_bCluster(bCluster),
	_bMask(true),
	_isValid(false),
	_isInitialized(false),
	_isProcessed(false)
{
	assert(image.isInitialized() && 3 == image._nChannels);
	assert(mask.isInitialized() && 1 == mask._nChannels);

	_imageOriginal = image;
	_mask = mask;
	_isValid = true;
	initialize();
}


inline 
void CSegmenter::setImage(const CIPPImage<Ipp8u> &image, int colorSpace, int coordinateSystem, bool bCluster)
{
	assert(image.isInitialized() && 3 == image._nChannels);

	_imageOriginal = image;
	_colorSpace = colorSpace;
	_coordinateSystem = coordinateSystem;
	_bCluster = bCluster;
	_bMask = false;
	_isValid = true;
	initialize();
}

inline 
void CSegmenter::setImage(const CIPPImage<Ipp8u> &image, const CIPPImage<Ipp8u> &mask, int colorSpace, int coordinateSystem, bool bCluster)
{
	assert(image.isInitialized() && 3 == image._nChannels);
	assert(mask.isInitialized() && 1 == mask._nChannels);

	_imageOriginal = image;
	_mask = mask;
	_colorSpace = colorSpace;
	_coordinateSystem = coordinateSystem;
	_bCluster = bCluster;
	_bMask = true;
	_isValid = true;
	initialize();
}


inline
void CSegmenter::load(CIPPImage<Ipp8u> image, int colorSpace, int coordinateSystem, bool bCluster)
{
	_imageOriginal = image;
	if(_imageOriginal.isInitialized())
	{	_isValid = true;
		_colorSpace = colorSpace;
		_coordinateSystem = coordinateSystem;
		_bCluster = bCluster;
		_bMask = false;
		_isValid = true;
		initialize();
	}
}

inline
void CSegmenter::load(const char * filename, int colorSpace, int coordinateSystem, bool bCluster)
{
	assert(NULL != filename);
	_imageOriginal.load(filename);
	if(_imageOriginal.isInitialized())
	{	_isValid = true;
		_colorSpace = colorSpace;
		_coordinateSystem = coordinateSystem;
		_bCluster = bCluster;
		_bMask = false;
		_isValid = true;
		initialize();
	}
}

inline
void CSegmenter::load(const char * filename, const char * filenameMask, int colorSpace, int coordinateSystem, bool bCluster)
{
	assert(NULL != filename);
	assert(NULL != filenameMask); 

	_imageOriginal.load(filename);
	_mask.load(filenameMask);
	if(_imageOriginal.isInitialized() && _mask.isInitialized()) // if has mask, mask needs to be initialized
	{	_isValid = true;
		_colorSpace = colorSpace;
		_coordinateSystem = coordinateSystem;
		_bCluster = bCluster;
		_bMask = true;
		_isValid = true;
		initialize();
	}
}

#endif // ___SEGMENTER__H___

