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
 * @file segmenter.cpp
 *
 * Primary Authors: Howard Zhou (howardz@cc.gatech.edu)\n
 *
 *
 * ==================================================================\n
 * @reference
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

#include <cstdlib>  // rand()
#include <ctime>	// time()


#include <iostream>
#include <fstream>

#include <algorithm>

#include <glib.h>

#include "Clustering/KMeans.h"
#include "segmenter.h"

#include "DataStructure/Signature.h"
#include "DissimilarityMeasure/EMDInterface.h"

using namespace std;

#ifndef INVALID_LABEL
#define INVALID_LABEL 255
#endif


#define RADIUS_IMPORTANCE_FACTOR 2
#define EXCEPTION_FACTOR 0.75

#define SMALL_REGION_THRESHOLD 900
#define NUM_SUPER_CLUSTERS 6

bool greaterRegion(const CRegion<CVector<float> > &rgn1, const CRegion<CVector<float> > &rgn2)
{
	return rgn1 > rgn2;
}


void CSegmenter::save(ImageMap &images, CDiamondContext &diamond)
{
	assert(_isProcessed);

	images["superClusters"] = _imageInSegs.LabToBGR();
	images["segmFilled"] = _segmFilled;
	images["segmFiltered"] = _segmFiltered;
}

void CSegmenter::segment(void)
{
	clusterKMeans(30, 40);

	// make sure the clusterCenter is already assigned
	assert(0 < _clusters.size());
	assert(_coordinateSystem = COORDINATESYSTEM_POLAR);
	int K = NUM_SUPER_CLUSTERS;
	int numIterations = 40;

	int numClusters = _clusters.size();
	vector<CVector<float> > clusterCenters;
	clusterCenters.reserve(numClusters);
	for(int i = 0; i < numClusters; i++)
	{	
		clusterCenters.push_back(_clusters[i]._featureMean);
		clusterCenters[i][3] *= RADIUS_IMPORTANCE_FACTOR;
	}

	// run fast K-means algorithm
	CKMeans<float> km(clusterCenters, K, numIterations);
	km.run();
	vector<CVector<float> > superClusterCenters = km.getCenters();

	// construct the super clusters
	int numSuperClusters = superClusterCenters.size();
	_superClusters.reserve(numSuperClusters);
	for(int i = 0; i < numSuperClusters; i++)
	{
		superClusterCenters[i][3] /= RADIUS_IMPORTANCE_FACTOR;
		_superClusters.push_back(CRegion<CVector<float> >(superClusterCenters[i], i));
	}

	// -------------------------------------------------------------
	// according to the cluster centers, assign each pixel a cluster label
	// -------------------------------------------------------------
	// establish cluster and superCluster connection.
	assignParent(_clusters, _superClusters);
	CIPPImage<Ipp8u> segMap = assignSuperClusterLabels(_clusterMap, _clusters, _superClusters, _imageInSegs);
	_imageInSegs = _imageInSegs.filterMedian(ippMskSize5x5);
	
	for(unsigned int i = 0; i < _superClusters.size(); i++){cout << _superClusters[i] << endl;}
	
	// classify the foreground and back ground pixels.
	CIPPImage<Ipp8u> foregroundMap = assignForegroundLabelsTest(_superClusters, _clusters, segMap, _clusterMap, _coordinateSystem);
	foregroundMap = cleanMask(foregroundMap);

	// commit to make color conversion
	commit();
	_segmFiltered = _imageInClusters;
	_segmFilled   = foregroundMap * 255;

	_mask		= _segmFilled;

	_isProcessed = true;
}

CIPPImage<Ipp8u> CSegmenter::assignForegroundLabelsTest(vector<CRegion<CVector<float> > > &superClusters,
													vector<CRegion<CVector<float> > > &clusters,
													const CIPPImage<Ipp8u> &superClusterMap,
													const CIPPImage<Ipp8u> &clusterMap,
													int cooridnateSystem)
{
	unsigned int numSuperClusters = superClusters.size();
	unsigned int numClusters = clusters.size();

	// sort the superClusters according to the polar coordinate system 
	for(unsigned int i = 0; i < numSuperClusters; i++)
		superClusters[i].setFeatureOfInterest(3);

	// rank the list cluster list according to there radius
	sort(superClusters.begin(), superClusters.end(), greaterRegion);
	
	for(unsigned int i = 0; i < numSuperClusters; i++)
		superClusters[i].setLabelSorted(i);
	//cout << endl << "after" << endl
	//	 << superClusters << endl;

	vector<int> indices;

	// ===============================================================
	// Remove the camera apture
	// ---------------------------------------------------------------
	// we assume that the inner most super cluster is never the camera
	for(unsigned int i = 0; i < numSuperClusters-1; i++)
	{	if(!isCamera2(superClusters[i]))	// if the radius is too big, inferring a ring shaped object
			break;	
		else
		{	superClusters[i].select();
			indices.push_back(superClusters[i]._label);
		}
	}
	CIPPImage<Ipp8u> cameraMask = showClusterMap(superClusterMap, indices);
	
	// now we exam the skin
	// we assume the scc indexed by the sccSkinStart is really the skin
	CIPPImage<Ipp8u> skinMask = generateSkinMaskStatistical2(superClusterMap, superClusters);

	// the rest of the super clusters having index >= lesionStart
	// are all considered as lesions.
	// However, we check for all the cluster whether they are too far away from the center
	// if it is too outside by a pre-set threshold, we consider that as mask
	CIPPImage<Ipp8u> exceptionMask;
	if(COORDINATESYSTEM_POLAR == cooridnateSystem)
	{
		indices.clear();
		float exceptionThreshold = 1/COORDINATE_TO_COLOR_RATIO_LAB * EXCEPTION_FACTOR;
		for(unsigned int i = 0; i < numClusters; i++)
			if(clusters[i]._featureMean[3] > exceptionThreshold)
				indices.push_back(i);
		exceptionMask = showClusterMap(clusterMap, indices);
	}

	// lesion mask will be the mask that excludes all of the above
	// cameraMask, skinMask, and exceptionMask
	CIPPImage<Ipp8u> foregroundMap(clusterMap._size, 1, Ipp8u(1));
	//foregroundMap = cameraMask.invertBinaryMask() * skinMask.invertBinaryMask();
	foregroundMap = cameraMask.invertBinaryMask() * skinMask.invertBinaryMask() * exceptionMask.invertBinaryMask();

	return foregroundMap;
}



// generateSkinMask
// 
// We generate skin mask in this function
// 1. we assume the scc center indexed by sccSkinStart is a definite patch of skin
//    and use it as our skin model
// 2. from outside to inside, we try to merge it with new scc
//    while(there are still one scc left)
//        if can merge
//            new_center = average(all_skin_centers)
//        else
//            skip
//
//

CIPPImage<Ipp8u> CSegmenter::generateSkinMaskStatistical2(	const CIPPImage<Ipp8u> &superClusterMap, 
												vector<CRegion<CVector<float> > > &superClusters) const
{
	// make a skin color model
	// black color
	int numScs = int(superClusters.size());
	vector<int> indices;
	CIPPImage<Ipp8u> skinMask;

	// ===============================================================
	// case 3.
	// if in between
	// randomly sample 100 points, generate signature and use EMC to 
	// determin if they are the same patch
	// generate the current skin mask

	// number of colors that are not skin


	CIPPImage<Ipp8u> scMask;
	vector<CVector<float> > pixels;
	vector<CSignature<CVector<float> > > scSignatures(numScs);

	int numPixels = 200;
	bool bSkin = false;
	int idxSkinStart = 0;
	for(int i = 0; i < numScs; i++)
	{
		// if this cluster is empty, it's not choosen
		if(superClusters[i].isSelected())	
			continue;

		if(false == bSkin && superClusters[i]._size > 2000)
		{
			bSkin = true;
			idxSkinStart = i;
		}

		scMask = showClusterMap(superClusterMap, vector<int>(1, superClusters[i]._label));	
		pixels = samplePixels(_image, numPixels, COORDINATESYSTEM_NONE, _coordinateToColorRatio, true, scMask);
		// make signature with these smapled pixels
		scSignatures[i] = CSignature<CVector<float> >(numPixels, pixels);
	}

	CSignature<CVector<float> > sigSkinStart = scSignatures[idxSkinStart];

	// calculate the distance between all the clusters with the first skin cluster
	for(int i = 0; i < numScs-1; i++)
	{
		if(superClusters[i].isSelected())
		{
			superClusters[i].deselect();
			continue;
		}

		if(idxSkinStart == i || earthMoversDistance(scSignatures[i], sigSkinStart) < 30)
		{	superClusters[i].select();
			indices.push_back(superClusters[i]._label);
		}
	}

	skinMask = showClusterMap(superClusterMap, indices);

	return skinMask;
}



bool CSegmenter::isCamera2(const CRegion<CVector<float> > &superClusters)
{
	// check for radius
	float radius = superClusters._featureMean[3] * _coordinateToColorRatio; 
	return (radius > 0.8);
}



CIPPImage<Ipp8u> showClusterMap(const CIPPImage<Ipp8u> &clusterMap,
								const vector<int> &clusterLabels)
{
	CIPPImage<Ipp8u> result(clusterMap._size, 1, Ipp8u(0));
	assert(1 == clusterMap._nChannels);
	int numClusterLabels = clusterLabels.size();
	for(int offset = 0; offset < clusterMap._size.height*clusterMap._size.width; offset++)
		for(int i = 0; i < numClusterLabels; i++)
			if(clusterLabels[i] == clusterMap._imageData[offset])
				result._imageData[offset] = 1;
	return result;
}

/**
 *
 *
 *
 **/
CIPPImage<Ipp8u> assignSuperClusterLabels(const CIPPImage<Ipp8u> &clusterMap,
										  const vector<CRegion<CVector<float> > > &clusters,
										  const vector<CRegion<CVector<float> > > &superClusters,
										  CIPPImage<Ipp8u> &imageInSuperClusters)
{
	CIPPImage<Ipp8u> result;
	CIPPImage<Ipp8u> superClusterMap(clusterMap._size, 1, Ipp8u(0));
	imageInSuperClusters = CIPPImage<Ipp8u>(clusterMap._size, 3);
	int label;
	for(int i = 0, offset3=0; i < clusterMap._size.height * clusterMap._size.width; i++, offset3+=3)
	{
		label = clusters[clusterMap._imageData[i]]._labelParent;
		superClusterMap._imageData[i] = label;
		
		// construct the cluster image
		// Intel IPP stores colored image in BGR format
		imageInSuperClusters._imageData[offset3]   = Ipp8u(superClusters[label]._featureMean[0]);
		imageInSuperClusters._imageData[offset3+1] = Ipp8u(superClusters[label]._featureMean[1]);
		imageInSuperClusters._imageData[offset3+2] = Ipp8u(superClusters[label]._featureMean[2]);
	}
	return superClusterMap;
}


/**
 *
 *
 *
 **/
void assignParent(vector<CRegion<CVector<float> > > &children,
				  vector<CRegion<CVector<float> > > &parents)
{
	int label;
	float dist, minDist;
	unsigned int numChildren = children.size();
	unsigned int numParents = parents.size();
	for(unsigned int i = 0; i < numChildren; i++)
	{
		minDist = FLT_MAX;
		// for each color cluster, get distance
		for(unsigned int j = 0; j < numParents; j++)
		{	
			dist = getNorm2Squared(children[i]._featureMean, parents[j]._featureMean);
			if(dist < minDist)
			{	minDist = dist;
				label = j;
			}
		}
		children[i].setLabelParent(label);
		parents[label].addLabelChildren(i);
		parents[label]._size += children[i]._size;
	}	
}

/**
 *
 *
 *
 **/
void CSegmenter::initialize(void)
{
	assert(_isValid);
	assert(3 == _imageOriginal._nChannels);

	switch(_colorSpace)
	{
	case COLORSPACE_CIE_LAB:
		_image = _imageOriginal.filterMedian(ippMskSize3x3).BGRToLab();
		_coordinateToColorRatio = COORDINATE_TO_COLOR_RATIO_LAB;
		break;
	case COLORSPACE_RGB:
	default:
		// Intel IPP arranges RGB images internally in the order of B G R
		_image = _imageOriginal.filterMedian(ippMskSize3x3).BGRToRGB();
		_coordinateToColorRatio = COORDINATE_TO_COLOR_RATIO_RGB;
	}
		
	_isInitialized = true;
}

void CSegmenter::commit(void)
{
	assert(_isValid);
	assert(3 == _imageOriginal._nChannels);

	switch(_colorSpace)
	{
	case COLORSPACE_CIE_LAB:
		if(_bCluster)
		{	_imageInClusters = _imageInClusters.LabToBGR();
			//_imageInSegs = _imageInSegs.LabToBGR();
		}
		break;
	case COLORSPACE_RGB:
	default:
		// Intel IPP arranges RGB images internally in the order of B G R
		if(_bCluster)
		{
			_imageInClusters = _imageInClusters.BGRToRGB();
			//_imageInSegs = _imageInSegs.BGRToRGB();
		}
	}
}

void CSegmenter::clusterKMeans(int K, int numIterations)
{
	assert(_isValid);

	_imageInClusters= CIPPImage<Ipp8u>(_image._size, 3);
	_clusterMap		= CIPPImage<Ipp8u>(_image._size, 1);

	// -------------------------------------------------------------
	// sample 10000 pixels and use K-means to determine the clusters
	// -------------------------------------------------------------
	// data points
	vector<CVector<float> > data = samplePixels(_image, 10000, _coordinateSystem, _colorSpace, _bMask, _mask);

	// run fast K-means algorithm
	CKMeans<float> km(data, K, numIterations);
	km.run();
	vector<CVector<float> > clusterCenters = km.getCenters();

	// construct the clusters based on the cluster centers
	_clusters.reserve(clusterCenters.size());
	for(unsigned int i = 0; i < clusterCenters.size(); i++)
		_clusters.push_back(CRegion<CVector<float> >(clusterCenters[i], i));

	// -------------------------------------------------------------
	// according to the cluster centers, assign each pixel a cluster label
	// -------------------------------------------------------------
	_clusterMap = assignClusterLabels(_image, _clusters, _imageInClusters, _coordinateSystem, _bMask, _mask);
}

CIPPImage<Ipp8u> CSegmenter::assignClusterLabels(const CIPPImage<Ipp8u> &image,
										   vector<CRegion<CVector<float> > > &clusters, 
										   CIPPImage<Ipp8u> &imageInClusters, 
										   int coordinateSystem,
										   bool bMask,
										   const CIPPImage<Ipp8u> &mask)
{
	assert(image.isInitialized() && 3 == image._nChannels);
	assert((bMask && mask.isInitialized() && 1 == mask._nChannels) || (!bMask));

	imageInClusters = CIPPImage<Ipp8u>(image._size, 3, Ipp8u(0));

	CIPPImage<Ipp8u> labelMap(image._size, 1);
	
	int width = image._size.width;
	int height = image._size.height;
	float dist, minDist;
	int label;
	int rowOffset, offset, offset3;
	int numClusters = clusters.size();

	// randomly generate x and y coordinates
	switch(coordinateSystem)
	{
	case COORDINATESYSTEM_POLAR:
		{
			float x1, y1;
			float xc = width/2.f;
			float yc = height/2.f;

			// normalized radius = r * 255 / r_image
			// the reason for x 255 is for it to be in the same range as the color values
			float rn = sqrt(float(xc * xc + yc * yc))*_coordinateToColorRatio;
			CVector<float> pixel(4, float(0));

			for(int y = 0; y < height; y++)
			{
				y1 = y - yc;
				rowOffset = y * width;

				for(int x = 0; x < width; x++)
				{	
					x1 = x - xc;
					offset = rowOffset + x;
					offset3 = offset * image._nChannels;
					if(bMask)
						if(!mask._imageData[offset])
						{
							labelMap._imageData[offset] = INVALID_LABEL;
							continue;
						}

					pixel[0] = image._imageData[offset3];		// blue
					pixel[1] = image._imageData[offset3+1];		// green
					pixel[2] = image._imageData[offset3+2];		// red
					pixel[3] = 	sqrt(float(x1*x1 + y1*y1))/rn;	// radius
					minDist = FLT_MAX;
					// for each color cluster, get distance
					for(int i = 0; i < numClusters; i++)
					{	
						dist = getNorm2Squared(pixel, clusters[i]._featureMean);
						if(dist < minDist)
						{	minDist = dist;
							label = i;
						}
					}
					// construct the label map
					labelMap._imageData[offset] = label;
					clusters[label]._size++;

					// construct the cluster image
					// Intel IPP stores colored image in BGR format
					imageInClusters._imageData[offset3]   = Ipp8u(clusters[label]._featureMean[0]);	// blue
					imageInClusters._imageData[offset3+1] = Ipp8u(clusters[label]._featureMean[1]);	// green
					imageInClusters._imageData[offset3+2] = Ipp8u(clusters[label]._featureMean[2]);	// red
				}
			}
		}
		break;
	default:
		{
			CVector<float> pixel(3, float(0));
			for(int y = 0; y < height; y++)
			{
				rowOffset = y * width;
				for(int x = 0; x < width; x++)
				{	
					offset = rowOffset + x;
					offset3 = offset * image._nChannels;
					if(bMask)
						if(!mask._imageData[offset])
						{
							labelMap._imageData[offset] = INVALID_LABEL;
							continue;
						}

					pixel[0] = image._imageData[offset3];		// blue
					pixel[1] = image._imageData[offset3+1];		// red
					pixel[2] = image._imageData[offset3+2];		// green

					minDist = FLT_MAX;
					// for each color cluster, get distance
					for(int i = 0; i < numClusters; i++)
					{	
						dist = getNorm2Squared(pixel, clusters[i]._featureMean);
						if(dist < minDist)
						{	minDist = dist;
							label = i;
						}
					}
					// construct the label map
					labelMap._imageData[offset] = label;
					clusters[label]._size++;

					// construct the cluster image
					// Intel IPP stores colored image in BGR format
					imageInClusters._imageData[offset3]   = Ipp8u(clusters[label]._featureMean[0]);	// blue
					imageInClusters._imageData[offset3+1] = Ipp8u(clusters[label]._featureMean[1]);	// green
					imageInClusters._imageData[offset3+2] = Ipp8u(clusters[label]._featureMean[2]);	// red
				}
			}
		}
		break;
	}
	return labelMap;	
}


// randomly sample pixels from the given image
vector<CVector<float> > samplePixels(const CIPPImage<Ipp8u> &image, 
									 int numPoints, 
									 int coordinateSystem, 
									 int colorSpace,
									 bool bMask,
									 const CIPPImage<Ipp8u> &mask)
{
	assert(image.isInitialized());
	assert(3 == image._nChannels);
	assert((bMask && mask.isInitialized() && 1 == mask._nChannels) || (!bMask));
	
	vector<CVector<float> > pixels;
	pixels.reserve(numPoints);

	float coordinateToColorRatio;
	switch(colorSpace)
	{
	case COLORSPACE_RGB		: coordinateToColorRatio = COORDINATE_TO_COLOR_RATIO_RGB;break;
	case COLORSPACE_CIE_LAB	: coordinateToColorRatio = COORDINATE_TO_COLOR_RATIO_LAB;break;
	default					: coordinateToColorRatio = 1;
	}
	// for color clustering, we don't want truly random sampling
	// we will always sample the same set of pixels as long as the distribution
	// is near uniform
	
	int width = image._size.width;
	int height = image._size.height;

	int offset, offset3, x, y;
	// randomly generate x and y coordinates
	switch(coordinateSystem)
	{
	case COORDINATESYSTEM_POLAR:
		{
			//CIPPImage<Ipp8u> sampleMap(image._size, image._nChannels, Ipp8u(0));
			float x1, y1;
			float xc = width/2.f;
			float yc = height/2.f;

			// normalized radius = r * 255 / r_image
			// the reason for x 255 is for it to be in the same range as the color values
			float rn = sqrt(float(xc * xc + yc * yc))*coordinateToColorRatio;
			CVector<float> pixel(4, float(0));
			for(int i = 0; i < numPoints; i ++)
			{
				x = rand() % width;		x1 = x - xc;
				y = rand() % height;	y1 = y - yc;
				offset = (y * width + x);
				offset3 = offset * image._nChannels;
				if(bMask)
					if(!mask._imageData[offset])
					{	i--; continue;	}
				
				pixel[0] = image._imageData[offset3];		// blue
				pixel[1] = image._imageData[offset3+1];		// green
				pixel[2] = image._imageData[offset3+2];		// red
				pixel[3] = 	sqrt(float(x1*x1 + y1*y1))/rn;	// radius
				
				pixels.push_back(pixel);

				//sampleMap._imageData[offset3] = pixel[0];
				//sampleMap._imageData[offset3+1] = pixel[1];
				//sampleMap._imageData[offset3+2] = pixel[2];

			}
		}
		break;
	default:
		{
			//CIPPImage<Ipp8u> sampleMap(image._size, image._nChannels, Ipp8u(0));
			CVector<float> pixel(3, float(0));
			for(int i = 0; i < numPoints; i ++)
			{
				x = rand() % width;
				y = rand() % height;
				offset = (y * width + x);
				offset3 = offset * image._nChannels;
				if(bMask)
					if(!mask._imageData[offset])
					{	i--; continue;	}
				
				pixel[0] = image._imageData[offset3];		// blue
				pixel[1] = image._imageData[offset3+1];		// green
				pixel[2] = image._imageData[offset3+2];		// red			
				pixels.push_back(pixel);

				//sampleMap._imageData[offset3] = pixel[0];
				//sampleMap._imageData[offset3+1] = pixel[1];
				//sampleMap._imageData[offset3+2] = pixel[2];
			}
		}
		break;
	}
	return pixels;	
}


CIPPImage<Ipp8u> cleanMask(const CIPPImage<Ipp8u> &mask)
{
	CIPPImage<Ipp8u> maskClean = mask.removeSmallRegions(SMALL_REGION_THRESHOLD);
	// first remove the holes inside the mask
	//maskClean = maskClean.invertBinaryMask().removeSmallRegions(SMALL_REGION_THRESHOLD).invertBinaryMask();

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

	IppiPoint anchor = {5, 5};
	IppiSize maskSize = {11, 11};


	//Ipp8u pMask[9*9] = { 0, 0, 1, 1, 1, 1, 1, 0, 0,
	//					0, 1, 2, 2, 2, 2, 2, 1, 0,
	//					1, 2, 2, 3, 3, 3, 2, 2, 1,
	//					1, 2, 3, 3, 4, 3, 3, 2, 1,
	//					1, 2, 3, 4, 4, 4, 3, 2, 1,
	//					1, 2, 3, 3, 4, 3, 3, 2, 1,
	//					1, 2, 2, 3, 3, 3, 2, 2, 1,
	//					0, 1, 2, 2, 2, 2, 2, 1, 0,
	//					0, 0, 1, 1, 1, 1, 1, 0, 0};
	//IppiPoint anchor = {4, 4};
	//IppiSize maskSize = {9, 9};

	maskClean = maskClean.morphCloseBorder(pMask, maskSize, anchor).filterMedian(ippMskSize5x5);
	maskClean = maskClean.invertBinaryMask().removeSmallRegions(SMALL_REGION_THRESHOLD*2).invertBinaryMask();
	//maskClean = maskClean.morphCloseBorder(pMask, maskSize, anchor).filterMedian(ippMskSize5x5);

	return maskClean;
}
