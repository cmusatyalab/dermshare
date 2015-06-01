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
 * Signature\n
 * @file Signature.h
 *
 * Primary Authors: Howard Zhou (howardz@cc.gatech.edu)\n
 *
 *
 * ==================================================================\n
 * @reference
 *
 * @version 1.0\n
 *     Initial Revision
 * @date 2007-10-16
 * @author Howard Zhou
 *
 *
 * @todo
 * @bug
 * @warning 
 *
 * ===================================================================\n
 */

#ifndef ___SIGNATURE__H___
#define ___SIGNATURE__H___

// #pragma warning(disable : 4786)

#include <vector>
#include <iomanip>

/// @namespace using standard namespace
using namespace std;

// ==================================================================================
/**
 * @class CSignature
 * @brief Signature
 * @ingroup Dissimilarity Measure
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
 * @date 2007-10-16
 * @author Howard Zhou
 *
 * 
 * @todo
 * @bug
 * @warning
 */
// ================================================================================

template<typename T>
class CSignature
{
public:
	int				_len;		///< the length of the vector
	vector<T>		_features;	///< pointer to the memory holding the data
	vector<float>	_weights;	

public:
	// ------------------------------------------------------
	// constructors and destructors
	// ------------------------------------------------------
	/**
	* Default Constructor\n
	* Creates a vector instance, but does not initialize it.
	*/
	CSignature(void)
		:_len(-1){};

	/// copy constructor
	// no dynamically allocated members, copy constructor not necessary
	//CSignature(const CSignature<T> &sig);

	CSignature(int length)
		:_len(length)
	{
		_features.reserve(length);
		_weights.reserve(length);
	};

	// using uniform weights
	CSignature(int length, const vector<T> &features)
		:_len(length),_features(features)
	{
		// using uniform weight if the weights argument is omitted.
		_weights = vector<float>(_len, 1.f/_len);
	};


	CSignature(int length, const vector<T> &features, const vector<float> &weights)
		:_len(length),_features(features),_weights(weights)
	{};

	//virtual ~CSignature(void);	// Destructor

	// ------------------------------------------------------
	// Predicators
	// ------------------------------------------------------
	/**
	 * A signature is valid if it is initialized and it's length is
	 * positive
	 * @return true if the signature is valid
	 */
	bool isValid (void) const 
	{
		return (_len > 0 && _len == (int)_features.size() && _len == (int)_weights.size());
	};
	// virtual bool isCompatible   (const CSignature<T>& sig) const;

	// ------------------------------------------------------
	// Accessors and Mutators
	// ------------------------------------------------------
	int			length			(void) const {return _len;}; // return int: length (dimention) of vector (number of elements).

	// normalize the weights
	CSignature<T>& normalizeWeights	(void);

	void		invalidate      (void) {_len = -1;};
	// ------------------------------------------------------
	// Overloaded Operators 
	// ------------------------------------------------------
	T&				operator[] (int i);
	T				operator[] (int i) const;
	//CSignature<T>&operator=  (const  CSignature<T>& sig);	// using default assignment operator
	bool			operator== (const  CSignature<T>& sig) const;
	bool			operator!= (const  CSignature<T>& sig) const;
	//bool			operator<  (const  CSignature<T>& sig) const;

	// ------------------------------------------------------
	// Output or Test
	// ------------------------------------------------------
	// static so that no object initialization is required to run this equation
	static void	test();

	// ------------------------------------------------------
	// Friend functions
	// ------------------------------------------------------
	/**
	* Provides special formatting when a matrix or vector (respectively) is part of the output stream.
	* @param os reference to the output stream	
	* @param sig reference to a vector
	* @return reference to the output stream
	*/
	friend ostream& operator<< (ostream& os, const CSignature<T>& sig)
	{
		assert(sig.isValid());
		os << "[ ";
		os << setprecision(3);
		os << setw(8);
		os << setiosflags(ios_base::fixed | ios_base::right);
		for(int i=0; i<sig._len; i++)
		{
			os << sig._features[i] << "(" << sig._weights[i] << ")\n";
		}
		os << "] ";
		return os;
	}
};



/////////////////////////////////////////////////////////
/////////////////// implementation //////////////////////
/////////////////////////////////////////////////////////


// ======================================================
// Actual function definition for CSignature
// ======================================================

// ------------------------------------------------------
// constructors and destructors
// ------------------------------------------------------

// ------------------------------------------------------
// Predicators
// ------------------------------------------------------


// ------------------------------------------------------
// Overloaded Operators 
// ------------------------------------------------------
 
/**
 * operator [] \n
 * @param i index
 * @return the refernce to the current vecotr element
 */
template<typename T>
inline T& CSignature<T>::operator[] (int i)
{
	assert(isValid() && i >= 0 && i< _len);
	return _features[i];
}

/**
 * operator [] (Read only) \n
 * Returns vector element value specified by index i (Read Only).
 * @param i index
 * @return the value of the current vector element
 */
template<typename T>
inline T CSignature<T>::operator[] (int i) const
{
	assert(isValid() && i >= 0 && i< _len);
	return _features[i];
}

/**
 * operator ==\n
 * @param sig reference to a vector  
 * @return true if all elements of sig are equal 
 * to the corresponding elements of the current vector.
 */
template<typename T>
inline bool CSignature<T>::operator== (const CSignature<T>& sig) const 
{
	assert(isValid() && sig.isValid());
	for(int i=0; i < _len; i++)
		if(_features[i] != sig._features[i] || _weights[i] != sig._weights)
			return false;
}

/**
 * operator !=\n
 * @param sig reference to a vector  
 * @return true if any element of sig is not equal 
 * to the corresponding element of the current vector.
 */
template<typename T>
inline bool CSignature<T>::operator!= (const CSignature<T>& sig) const 
{
	assert(isValid() && sig.isValid());
	return !(*this == sig);
}


/**
 * normalizeWeights
 */
template<typename T>
CSignature<T>& CSignature<T>::normalizeWeights(void)
{
	assert(isValid());
	int i;
	float sum = 0;
	for(i = 0;  i < _len; i++)
		sum += _weights[i];
	
	for(int i = 0; i < _len; i++)
		_weights[i] /= sum;

	return (*this);
}

///**
// * operator <\n
// * This operator is defined so this class can be used in 
// * STL containers.
// * @param sig reference to a vector  
// * @return true if the norm2 of the sig is smaller
// * than the current one.
// */
//template<typename T>
//inline bool CSignature<T>::operator< (const CSignature<T>& sig) const 
//{
//	assert(isVaid() && sig.isValid());
//	return false;
//}


#endif // ___SIGNATURE__H___
