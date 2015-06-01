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

#ifndef ____EMD_INTERFACE__H____
#define ____EMD_INTERFACE__H____

#include "DataStructure/Signature.h"
#include "EMD.h"


float dist(feature_t *f1, feature_t *f2);

template<typename Scalar>
float earthMoversDistance(const CSignature<Scalar> & sig1, const CSignature<Scalar> & sig2);

void testEMD();


// If a template function is not put in the .h file, it might raise a undefined reference error
template<typename Scalar>
float earthMoversDistance(const CSignature<Scalar> & sig1, const CSignature<Scalar> & sig2)
{
	signature_t signature1;
	signature_t signature2;
	
	signature1.n = sig1.length();
	// allocate memory for n features
	signature1.Features = new feature_t[signature1.n];
	signature1.Weights = new float[signature1.n];

	for(int i = 0; i < signature1.n; i++)
	{
		// initialize each feature and its associated weight
		signature1.Features[i]._x	= sig1._features[i][0];
		signature1.Features[i]._y	= sig1._features[i][1];
		signature1.Features[i]._z	= sig1._features[i][2];
		signature1.Weights[i]		= sig1._weights[i];
	}	


	signature2.n = sig2.length();
	// allocate memory for n features
	signature2.Features = new feature_t[signature2.n];
	signature2.Weights = new float[signature2.n];

	for(int i = 0; i < signature2.n; i++)
	{
		// initialize each feature and its associated weight
		signature2.Features[i]._x	= sig2._features[i][0];
		signature2.Features[i]._y	= sig2._features[i][1];
		signature2.Features[i]._z	= sig2._features[i][2];
		signature2.Weights[i]		= sig2._weights[i];
	}	

	float emdScore =  emd(&signature1, &signature2, dist, NULL, NULL);

	delete[] signature1.Features;	signature1.Features = NULL;
	delete[] signature2.Features;	signature2.Features = NULL;
	delete[] signature1.Weights;	signature1.Weights	= NULL;
	delete[] signature2.Weights;	signature2.Weights	= NULL;

	return emdScore;
}


#endif //____EMD_INTERFACE__H____

