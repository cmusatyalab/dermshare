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
 * K-means Clustering \n
 * @file KMeans.h
 *
 * Primary Authors: Howard Zhou (howardz@cc.gatech.edu)\n
 *
 *
 * ==================================================================\n
 * @reference
 *
 * @version 1.0\n
 *     Initial Revision: Adoption from David Arthur
 * @date 2007-10-10
 * @author Howard Zhou
 *
 * @version 0.0\n
 *     Initial Revision
 * @date 2007-10-10
 * @author David Arthur
 *
 *
 * @todo
 * @bug
 * @warning 
 *
 * ===================================================================\n
 */

#ifndef ___KMEANS__H___
#define ___KMEANS__H___

// #pragma warning(disable : 4786)

#include <cassert>
#include <iostream>
#include <sstream>
#include <fstream>

#include <vector>
#include <string>

#include "math/Vector.h"


#include "ClusteringUtilities.h"

#ifndef ___KMEANS__H___COUT_LVL_1
#define ___KMEANS__H___COUT_LVL_1 1
#endif

#ifndef ___KMEANS__H___COUT_LVL_2
#define ___KMEANS__H___COUT_LVL_2 0
#endif



/// @namespace using standard namespace
using namespace std;


// ==================================================================================
/**
 * @class CKMeans
 * @brief K-means Clustering
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
 *     Initial Revision: Adoption from David Arthur
 * @date 2007-10-10
 * @author Howard Zhou
 *
 * @version 0.0\n
 *     Initial Revision
 * @date 2007-10-10
 * @author David Arthur
 * 
 * @todo
 * @bug
 * @warning
 */
// ================================================================================
template<typename Scalar>
class CKMeans
{
public:
	vector<CVector<Scalar> >	_data;
	vector<CVector<Scalar> >	_centers;
	vector<int>					_labels;
	Scalar						_potential;

	int							_K;
	int							_numIterations;

	/// tuning parameters
	float		_scale;

	/// state flags
	bool		_isValid;
	bool		_isInitialized;
	bool		_isProcessed;

	/// results

public:
	// ------------------------------------------------------
	// constructors and destructors
	// ------------------------------------------------------
	/// default constructor
	CKMeans()
		:_isValid(false),_isInitialized(false),_isProcessed(false)
	{};
	
	// use member initialization list.
	CKMeans(const vector<CVector<Scalar> > &data, int K, int numIterations)
		:_data(data),_K(K),_numIterations(numIterations),
		_isValid(true),_isInitialized(true),_isProcessed(false)
	{};

	/// copy constructor
	// since we don't have dynamically allocated member, copy constructor is not necessary
	//CKMeans(const CKMeans<Scalar> & p){}
		
	/// destructor
	~CKMeans(){};

	/// initializer
	void initialize(void);

	// ------------------------------------------------------
	// Predicators
	// ------------------------------------------------------
	bool isValid		()const { return _isValid;		};
	bool isInitialized	()const { return _isInitialized;};
	bool isProcessed	()const { return _isProcessed;	};

	// ------------------------------------------------------
	// Accessors and Mutators
	// ------------------------------------------------------
	vector<CVector<Scalar> >	getCenters	(void){ return _centers;	};
	vector<int>					getLabels	(void){ return _labels;		};	

	// ------------------------------------------------------
	// Overloaded Operators 
	// ------------------------------------------------------

	// ------------------------------------------------------
	// Input, Output, Test
	// ------------------------------------------------------
	void load(const char * filename);
	void save(const char * filename);
	
	static void test(void); 

	// ------------------------------------------------------
	// Other operation
	// ------------------------------------------------------
	void run(void);

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

template<typename Scalar>
void CKMeans<Scalar>::initialize(void)
{
	assert(_isValid);
	_isInitialized = true;
}


template<typename Scalar>
void CKMeans<Scalar>::run(void)
{
	assert(_isInitialized);

	int numTrials = 1;
	int measurementInterval = 5;

	assert(_numIterations % measurementInterval == 0);

#if ___KMEANS__H___COUT_LVL_1
 	cout << endl << "=================================="
		 << endl << "Runing K-Means++ (" << _K << " clusters) ..." << endl;
#endif

	//string logFilename = "log.txt";

 //   // Open the log file
 //   cout << "Outputting this test case to log file: \"" << logFilename << "\"." << endl << endl;
    //ofstream out(logFilename.c_str());
    //if (out.fail()) {
    //    ostringstream error;
    //    error << "Could not write to log file \"" << logFilename << "\".";
    //}

    // Output the title

	// ostringstream title;
	//title << "RUNNING KMEANS++, looking for " << _K << " clusters";
    //out << title.str() << endl;
    //for (unsigned int i = 0; i < title.str().size(); i++)
    //    out << "=";
    //out << endl << endl;

	// Set everything up
	vector<Scalar> measuredPotential, trialPotential;
	if (measurementInterval == -1)
		measurementInterval = _numIterations;
	Scalar bestMeasuredPotential = -1;
	Scalar maxTrialPot = 0, totalTrialPot = 0;
	Scalar minTrialPot = 1e20;
	int curTrial = 0, curIteration = 0;

	// Iterate as necessary
	
	while (curIteration < _numIterations || curTrial < numTrials) {
		int stepSize = measurementInterval;

		// Iterate stepSize iterations
		do {
			// Initialize the clustering if appropriate
			if (_centers.size() == 0) {
				_centers = chooseCentersSmart(_data, _K, 1);
				stepSize--;
			}

			// Do the clustering if appropriate
			if (stepSize > 0) {
				_centers = runKMeans(_data, _centers, stepSize);
			}

			// Update our best results
			_labels = assignPointsToClusters(_data, _centers);
			_potential = getKMeansPotential(_data, _centers, _labels);
			
			if (curIteration < _numIterations) {
				if (bestMeasuredPotential < 0 || _potential < bestMeasuredPotential)
					bestMeasuredPotential = _potential;
			}

			// Reset the clustering if appropriate, and instrument this clustering trial
			if (stepSize != 0) {
				if (curTrial < numTrials) {
					minTrialPot = min(_potential, minTrialPot);
					maxTrialPot = max(_potential, maxTrialPot);
					totalTrialPot += _potential;
					curTrial++;
#if ___KMEANS__H___COUT_LVL_1
					cout << "Completed trial " << curTrial << " of " << numTrials 
						 << " (potential = " << _potential << ")..." << endl;
#endif
					trialPotential.push_back(_potential);
				}
				_centers.clear();
			}
		} while (stepSize > 0);

		// Instrument the iteration data
		if (curIteration < _numIterations) {
			measuredPotential.push_back(bestMeasuredPotential);
			curIteration += measurementInterval;

#if ___KMEANS__H___COUT_LVL_2
			cout << "Completed iteration " << curIteration << " of " << _numIterations 
				 << " (potential = " << bestMeasuredPotential << ")..." << endl;
#endif
		}
	}

	//// Output the summary over all trials
 //   out << "Potential average by trial: " << (totalTrialPot / numTrials) << endl;
 //   out << "Potential range by trial: " << minTrialPot << " to " << maxTrialPot << endl;

	//// Output the information for each trial
	//for (int i = 0; i < numTrials; i++)
 //       out << "Trial " << (i+1) << " of " << numTrials << ": "
 //           << trialPotential[i] << " potential." << endl;
	//out << endl;

	//// Output the information for each iteration
	//for (int i = 0; i < _numIterations/measurementInterval; i++)
	//	out << "Potential after " << (i+1)*measurementInterval << " iterations: " 
	//		<< measuredPotential[i] << endl;
	//out << endl << endl;

	//out.close();
	_isProcessed = true;
}

template<typename Scalar>
void CKMeans<Scalar>::load(const char * filename)
{
	_isValid = true;
	initialize();
}

template<typename Scalar>
void CKMeans<Scalar>::save(const char * filename)
{
	assert(_isProcessed);
}

template<typename Scalar>
void CKMeans<Scalar>::test(void)
{
	cout << "\nTesting K-means++ ..." << endl;
//	CKMeans<float> km;
//	km.initialize();
//	km.run();
}




#endif // ___KMEANS__H___


