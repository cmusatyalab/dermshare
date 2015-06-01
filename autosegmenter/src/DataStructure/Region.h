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
 * Region\n
 * @file Region.h
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
 * @date 2007-10-26
 * @author Howard Zhou
 *
 *
 * @todo
 * @bug
 * @warning 
 *
 * ===================================================================\n
 */

#ifndef ___REGION__H___
#define ___REGION__H___

// #pragma warning(disable : 4786)
#include <vector>

/// @namespace using standard namespace
using namespace std;


// ==================================================================================
/**
 * @class CRegion
 * @brief A general region base class
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
 * @date 2007-10-20
 * @author Howard Zhou
 *
 * 
 * @todo
 * @bug
 * @warning
 */
// ================================================================================

template<typename T>
class CRegion
{
public:
	// labels 
	int				_label;
	int				_labelSorted;		//< temporary label holder after sorting.
	int				_labelParent;
	vector<int>		_labelChildren;

	// region features 
	T				_featureMean;		//< mean feature value of this region
	vector<T>		_featureSamples;	//< sampled features of this region

	// feature to compare
	int				_idxFOI;			//< the index of the feature of interest
										//< used for comparison

	// global region properties
	float			_size;				//< area in 2D, volumn in 3D
	
protected:
	// predicate
	bool			_isValid;
	bool			_isInitialized;
	bool			_isSelected;

public:
	// ------------------------------------------------------
	// constructors and destructors
	// ------------------------------------------------------
	/// default constructor
	CRegion(void):
	  _label(-1),_labelSorted(-1),_labelParent(-1),_idxFOI(0),_size(0),
	  _isValid(false),_isInitialized(false),_isSelected(false)
	{};

	 CRegion(T featureMean, int label, int labelParent = -1):
	  _label(label),_labelSorted(-1),_labelParent(labelParent),
	  _featureMean(featureMean),_idxFOI(0),_size(0),
	  _isValid(false),_isInitialized(false),_isSelected(false)
	{};

	// CRegion(vector<T> featureSamples, float size, int label, int labelParent = -1):
	//  _label(label),_labelSorted(-1),_labelParent(labelParent),
	//  _featureSamples(featureSamples),_idxFeature(0),_size(size),
	//  _isValid(false),_isInitialized(false)
	//{};

	/// copy constructor
	// no dynamically allocated variables, no need for copy constructor 
	// CRegion(const CRegion& rgn);

	/// virtual destructor
	  virtual ~CRegion(){};

	/// initializer
	// calculate the feature mean
	void initialize(void);

	// ------------------------------------------------------
	// Predicators
	// ------------------------------------------------------
	bool isValid		(void)const { return _isValid;		};
	bool isInitialized	(void)const { return _isInitialized;};
	bool isSelected		(void)const { return _isSelected;	};

	void invalidate		(void)		{ _isValid = false;		};
	void select			(void)		{ _isSelected = true;	};
	void deselect		(void)		{ _isSelected = false;	};
	// ------------------------------------------------------
	// Accessors and Mutators
	// ------------------------------------------------------
	T getFeatureMean	(void)const { return _featureMean;	};
	float getSize		(void)const { return _size;			};


	// Mutators
	void setSize		(float size){ _size = size;			};
	void setLabelParent	(int label)	{ _labelParent = label;	};
	void setLabelSorted (int label)	{ _labelSorted = label; };
	void setFeatureOfInterest(int idxFeature){_idxFOI = idxFeature;	};

	void addLabelChildren(int label){_labelChildren.push_back(label);};
	void incrementSize	(void)		{ _size ++;				};

	// ------------------------------------------------------
	// Overloaded Operators 
	// ------------------------------------------------------
	bool operator< (const CRegion<T>& rgn) const
	{
		return (_featureMean[_idxFOI] < rgn._featureMean[_idxFOI]);	
	}

	bool operator> (const CRegion<T>& rgn) const
	{
		return (_featureMean[_idxFOI] > rgn._featureMean[_idxFOI]);	
	}

	// ------------------------------------------------------
	// Input, Output, Test
	// ------------------------------------------------------
	void load(const char * filename);
	void save(const char * filename);
	
	static void test(void); 

	// ------------------------------------------------------
	// Other operation
	// ------------------------------------------------------


	// ------------------------------------------------------
	// Friend functions
	// ------------------------------------------------------
	/**
	* Provides special formatting when a matrix or vector (respectively) is part of the output stream.
	* @param os reference to the output stream	
	* @param vec reference to a vector
	* @return reference to the output stream
	*/
	friend ostream& operator<< (ostream& os, const CRegion<T>& rgn)
	{
		os << "Region (" << rgn._label << ") : " << rgn._size << " | " << rgn._featureMean;
		return os;
	}


private:
	// ------------------------------------------------------
	// private helpers
	// ------------------------------------------------------

	// main functional steps
	// ------------------------------------------------------

	// other helpers
	// ------------------------------------------------------

};


// ------------------------------------------------------
// constructors and destructors
// ------------------------------------------------------
///// virtual destructor
//template<typename T> 
//virtual inline CRegion::~CRegion()
//{
//	if(_labelChildren)
//		delete[] _labelChilren;
//
//	if(_featureSamples)
//		delete[] _featureSamples;
//}





#endif // ___REGION__H___





