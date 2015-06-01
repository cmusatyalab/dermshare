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

#include <iostream>

#include <vector>

#include "math/Vector.h"
#include "EMDInterface.h"



using namespace std;

float dist(feature_t *f1, feature_t *f2)
{
	// assume that we are always in R3 space
	float dx = f1->_x-f2->_x, dy = f1->_y-f2->_y, dz = f1->_z-f2->_z;
	return sqrt(float(dx*dx + dy*dy + dz*dz));
}


void testEMD()
{
	vector<CVector3<float> > f1;
	f1.push_back(CVector3<float>(100,  40,  22));
	f1.push_back(CVector3<float>(211,  20,   2));
	f1.push_back(CVector3<float>( 32, 190, 150));
	f1.push_back(CVector3<float>(  2, 100, 100));

	vector<CVector3<float> > f2;
	f2.push_back(CVector3<float>(  0,   0,   0));
	f2.push_back(CVector3<float>( 50, 100,  80));
	f2.push_back(CVector3<float>(255, 255, 255));

	vector<float> w1;
	w1.push_back(0.4);
	w1.push_back(0.3);
	w1.push_back(0.2);
	w1.push_back(0.1);

	vector<float> w2;
	w2.push_back(0.5);
	w2.push_back(0.3);
	w2.push_back(0.2);

	CSignature<CVector3<float> > sig1(4, f1, w1);
	CSignature<CVector3<float> > sig2(3, f2, w2);

	cout << earthMoversDistance(sig1, sig2) << endl;
}


