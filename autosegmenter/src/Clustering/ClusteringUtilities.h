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

#include <cassert>	// for assert()
#include <cfloat>	// for FLT_MAX

#include <vector>	// STL vector class

#include <stdlib.h>	// for rand()

#include "math/Vector.h"


#ifndef ___CLUSTERING_UTILITIES__H___
#define ___CLUSTERING_UTILITIES__H___


using namespace std;

double getTime(void);
/**
 * Randomization utilities
 **/
float getRandomScalar(void);
//Point getRandomPoint(int numDimensions);
//Point getByNormDist(const Point &center, Scalar variance);


/**
 * K-means utilities
 **/
template<typename Scalar> 
vector<int>				assignPointsToClusters	(const vector<CVector<Scalar> > &data, const vector<CVector<Scalar> > &centers);
template<typename Scalar> 
vector<CVector<Scalar> >computeCenters			(const vector<CVector<Scalar> > &data, const vector<int> &labels, int numClusters);
template<typename Scalar>
vector<CVector<Scalar> >chooseCentersSmart		(const vector<CVector<Scalar> > &data, int numCenters, int numLocalTrials);
template<typename Scalar>
vector<CVector<Scalar> >chooseCentersUniform	(const vector<CVector<Scalar> > &data, int numCenters);
template<typename Scalar>
vector<CVector<Scalar> >runKMeans				(const vector<CVector<Scalar> > &data, const vector<CVector<Scalar> > &initCenters, int &numIterations);
template<typename Scalar>
Scalar					getKMeansPotential		(const vector<CVector<Scalar> > &data, const vector<CVector<Scalar> > &centers, const vector<int> &labels);


/**
 * Returns a number uniformly at random from [0,1).
 **/
inline float getRandomScalar() {
	return float(rand()) / RAND_MAX;
}



/**
 * Given fixed data points and fixed centers, this returns the index of which center is closest
 * to each point.
 **/
template<typename Scalar> 
vector<int> assignPointsToClusters(const vector<CVector<Scalar> > &data, 
								   const vector<CVector<Scalar> > &centers) 
{
	vector<int> result(data.size());	// reserve the space
	unsigned int numData = data.size();
	unsigned int numCenters = centers.size();
	for (unsigned int i = 0; i < numData; i++) 
	{
		Scalar bestValue = FLT_MAX;
		int bestIndex;
		for (unsigned int j = 0; j < numCenters; j++) {
			Scalar thisValue = getNorm2Squared(data[i], centers[j]);
			if (thisValue < bestValue) {
				bestValue = thisValue;
				bestIndex = j;
			}
		}
		result[i] = bestIndex;
	}
	return result;
}


/** 
 * Given a collection of cluster centers, this assigns each data point to the closest cluster center
 * and computes the k-means potential for this clustering.
 **/
template<typename Scalar>
inline Scalar getKMeansPotential(const vector<CVector<Scalar> >&data, 
								 const vector<CVector<Scalar> >&centers, 
								 const vector<int>				&labels) 
{
	Scalar total = 0;
	unsigned int numData = data.size();
    for (unsigned int i = 0; i < numData; i++)
        total += getNorm2Squared(data[i], centers[labels[i]]);
    return total / numData;
}


/**
 * Given fixed data points and a fixed cluster assignment, this returns the optimal centers for this
 * cluster assignment.
 **/
template<typename Scalar>
vector<CVector<Scalar> > computeCenters(const vector<CVector<Scalar> > &data, 
										const vector<int> &labels, 
										int numClusters)
{
	vector<int> clusterSize(numClusters);
	vector<CVector<Scalar> > clusterPosition(numClusters, CVector<Scalar>(data[0].length(), Scalar(0)));
	vector<CVector<Scalar> > result;

	int numLabels = labels.size();
	for (int i = 0; i < numLabels; i++) {
		clusterSize[labels[i]]++;
		// HOWARD 2007-10-13
		// By simply changing to +=, we shaved about 4 seconds off of 14 seconds on a 300x225 images
		//clusterPosition[labels[i]] = clusterPosition[labels[i]] + data[i];
		clusterPosition[labels[i]] += data[i];
	}

	for (int i = 0; i < numClusters; i++)
		if (clusterSize[i] > 0)
			result.push_back(clusterPosition[i] / clusterSize[i]);
	return result;
}


/**
 * Runs k-means on the given data points with the given initial centers.
 * It iterates up to numIterations times, and for each iteration it does, it decreases numIterations by 1.
 **/
template<typename Scalar>
vector<CVector<Scalar> > runKMeans(const vector<CVector<Scalar> > &data, 
								   const vector<CVector<Scalar> > &initCenters, 
								   int &numIterations) 
{
	vector<CVector<Scalar> > centers = initCenters;
	vector<int> clusterAssignment = assignPointsToClusters(data, centers);
	bool isProgress = false;

	while (numIterations > 0) {
		centers = computeCenters(data, clusterAssignment, centers.size());
		vector<int> newClusterAssignment = assignPointsToClusters(data, centers);
		isProgress = false;
		for (unsigned int i = 0; i < clusterAssignment.size(); i++)
			if (clusterAssignment[i] != newClusterAssignment[i])
				isProgress = true;
		if (!isProgress)
			break;
		clusterAssignment = newClusterAssignment;
		numIterations--;
	}

	return centers;
}

/**
 * Chooses a number of centers from the data set as follows:
 *  - One center is chosen randomly.
 *  - Now repeat numCenters-1 times:
 *      - Repeat numLocalTries times:
 *          - Add a point x with probability proportional to the distance squared from x
 *            to the closest existing center
 *      - Add the point chosen above that results in the smallest potential.
 **/
template<typename Scalar>
vector<CVector<Scalar> > chooseCentersSmart(const vector<CVector<Scalar> >&data, 
											int numCenters, int numLocalTries)
{
	assert(numCenters > 0 && numCenters <= (int)data.size() );
    Scalar currentPot = 0;
    vector<CVector<Scalar> > centers;
	vector<Scalar> closestDistSq;

	unsigned int numData = data.size();
	// Choose one random center and set the closestDistSq values
    unsigned int index = (int)(getRandomScalar() * numData);
	centers.push_back(data[index]);
    for (unsigned i = 0; i < numData; i++) {
		closestDistSq.push_back( getNorm2Squared(data[i], data[index]) );
        currentPot += closestDistSq[i];
    }

	// Choose each center
	for (int centerCount = 1; centerCount < numCenters; centerCount++) 
	{
        // Repeat several trials
        Scalar bestNewPot = FLT_MAX;
        int bestNewIndex;
        for (int localTrial = 0; localTrial < numLocalTries; localTrial++) {
		
    		// Choose our center - have to be slightly careful to return a valid answer even accounting
			// for possible rounding errors
		    Scalar randVal = getRandomScalar() * currentPot;
			
            for (index = 0; index < numData-1; index++) {
                if (randVal <= closestDistSq[index])
                    break;
                else
                    randVal -= closestDistSq[index];
            }

    		// Compute the new potential
            Scalar newPot = 0;
		    for (unsigned int i = 0; i < numData; i++)
                newPot += min( getNorm2Squared(data[i], data[index]), closestDistSq[i] );

            // Store the best result
            if (newPot < bestNewPot) 
			{
                bestNewPot = newPot;
                bestNewIndex = index;
            }
		}

        // Add the appropriate center
        centers.push_back(data[bestNewIndex]);
        currentPot = bestNewPot;
        for (unsigned i = 0; i < numData; i++)
            closestDistSq[i] = min( getNorm2Squared(data[i], data[bestNewIndex]), closestDistSq[i] );
	}
		
	return centers;
}


#endif //___CLUSTERING_UTILITIES__H___

