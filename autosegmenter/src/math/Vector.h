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
 * MathLib\n
 * @file Vector.h
 *
 * Primary Author: Howard Zhou (howardz@cc.gatech.edu)\n
 *
 *
 * ==================================================================\n
 * Basic Vector lab
 *
 * @version 1.3\n
 * @date 2006-10-20
 *     1. added p-norm for Vector class
 * @author howardz
 *
 * @version 1.2\n
 *     1. added template for Vecotr class
 *     2. replaced all division by scaler with multiplication by it's inverse
 *     3. removed .cpp file, all function definition are in .h file now
 * @date 2005-12-16
 * @author howardz
 *
 * @version 1.1\n
 *     Major organization change, moved most of function defintion to\n
 *     the .cpp file to make class declaration more clear.
 * @date 2003-05-14
 * @author howardz
 *
 * @version 1.0\n
 *     Initial Revision
 * @date 2003-02-23
 * @author howardz
 *
 *
 * @todo Implement det() and invert() 
 * @bug
 * @warning 
 *
 * ===================================================================\n
 */

#ifndef ___VECTOR__H___
#define ___VECTOR__H___

#include <cstdio>
#include <cassert>
#include <cmath>

#include <cstring>
#include <string>
#include <iostream>
#include <iomanip>

//#include "Matrix.h"

/// @namespace using standard namespace
using namespace std;

// forward declaration
//template<typename T>
//class CMatrix<T>;

// ==================================================================================
/**
 * @class CVector Vector.h Vector.h
 * @brief General Vector class
 * 
 * @ingroup Lib
 *
 * @par requirements
 * MFC\n
 *
 * @par license
 * This code is absolutely free to use and modify. The code is provided "as is" with
 * no expressed or implied warranty. The author accepts no liability if it causes
 * any damage to your computer, causes your pet to fall ill, increases baldness
 * or makes your car start emitting strange noises when you start it up.
 * This code has no bugs, just undocumented features!
 * 
 * @version 1.1\n
 *     Major organization change, moved most of function defintion to the .cpp\n
 *     file to make class declaration more clear.
 * @date 2003-05-14
 * @author howardz
 *
 * @version 1.0\n
 *     Initial Revision
 * @date 2003-02-23
 * @author howardz
 *
 *
 * @todo none
 * @bug none
 * @warning none
 */
// ================================================================================
template<typename T>
class CVector
{
protected:
	int _len;         ///< the length of the vector
	T *_v;       ///< pointer to the memory holding the data

public:
	// ------------------------------------------------------
	// constructors and destructors
	// ------------------------------------------------------
	/**
	* Default Constructor\n
	* Creates a vector instance, but does not initialize it.
	*/
	CVector(void):
		_len(-1),_v(NULL){}

	/// copy constructor
	CVector(const CVector<T> &vec);

	CVector(int length, T elValue);
	CVector(int length, T *storage = NULL);

	// convert the matrix into a vector
	// depending whether its row major or column major
	//CVector(const CMatrix<T> &mat, bool major = 0);

	virtual ~CVector(void);	// Destructor
	//~CVector(void);	// Destructor

	// ------------------------------------------------------
	// Predicators
	// ------------------------------------------------------
	virtual bool isValid        (void) const;
	virtual bool isCompatible   (const CVector<T>& vec) const;
	//bool isValid        (void) const;
	//bool isCompatible   (const CVector<T>& vec) const;
	// ------------------------------------------------------
	// Accessors and Mutators
	// ------------------------------------------------------
	int			length			(void) const {return _len;}; // return int: length (dimention) of vector (number of elements).
	T*			data			(void) const {return _v;};
    CVector<T>& normalize		(void);
    CVector<T>  normalized		(void) const;
	CVector<T>& reverse			(void);
	CVector<T>  reversed		(void) const;

	T			norm			(char p = '2') const;
	T			norm1			(void) const;
	T			norm2			(void) const;
	T			norm2Squared	(void) const;
	T			normInfinity	(void) const;

	void		invalidate      (void) {_len = -1;};
	// ------------------------------------------------------
	// Overloaded Operators 
	// ------------------------------------------------------
	T&			operator[] (int i);
	T			operator[] (int i) const;
	CVector<T>&	operator=  (T value); 
	CVector<T>&	operator=  (const  CVector<T>& vec);
	CVector<T>	operator-  (void)  const;
	CVector<T>	operator+  (const  CVector<T>& vec) const;
	CVector<T>	operator-  (const  CVector<T>& vec) const;
	CVector<T>	operator*  (T s) const;
	T			operator*  (const  CVector<T>& vec) const;	// this is defined as the dot product here
	CVector<T>	operator/  (T s) const;
	CVector<T>&	operator+= (const  CVector<T>& vec);
	CVector<T>&	operator-= (const  CVector<T>& vec);
	CVector<T>&	operator*= (T s);
	CVector<T>&	operator/= (T s);
	bool		operator== (const  CVector<T>& vec) const;
	bool		operator!= (const  CVector<T>& vec) const;
	bool		operator<  (const  CVector<T>& vec) const;


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
	* @param vec reference to a vector
	* @return reference to the output stream
	*/
	friend ostream& operator<< (ostream& os, const CVector<T>& vec)
	{
		assert(vec.isValid());
		os << "[ ";
		for(int i=0; i<vec._len; i++)
		{
			os << setprecision(3);
			os << setw(8);
			os << setiosflags (ios_base::fixed | ios_base::right);
			os << vec._v[i] << " ";
		}
		os << "] ";
		return os;
	}

	// calculate the squared distance
	friend inline T getNorm2Squared(const CVector<T> &v1, const CVector<T> &v2)
	{
		//assert(v1.isValid() && v2.isValid());
		//assert(v1._len == v2._len);

		T sum = 0, tmp;
		for(int i = 0; i< v1._len; i++)
		{	tmp = v1._v[i]-v2._v[i];
			sum += tmp*tmp;
		}
		return sum;
	}

};


/////////////////////////////////////////////////////////
/////////////////// implementation //////////////////////
/////////////////////////////////////////////////////////


// ======================================================
// Actual function definition for CVector
// ======================================================

// ------------------------------------------------------
// constructors and destructors
// ------------------------------------------------------

/**
 * Copy Constructor\n
 * Creates a vector instance with the number of elements and 
 * element values of vector vec.
 * @param &vec reference to a vector
 */
template<typename T>
inline CVector<T>::CVector(const CVector<T> &vec)
{
	assert(vec.isValid());

	_len = vec._len;

	_v = new T[_len];
	memcpy(_v, vec._v, _len * sizeof(T));
}
	
/**
 * Constructor\n
 * Creates a vector instance with length l and all of its elements
 * elValue
 * @param length the lenght of the vector
 * @param elValue the value to assign to the vector
 */
template<typename T>
inline CVector<T>::CVector(int length, T elValue)
{
	assert(length > 0);
	_len = length;

	_v = new T[length];

	for(int i=0; i< _len; i++)
		_v[i] = elValue;
}

/**
 * Constructor\n
 * Creates a dynamically sized T-precision vector with length elements.
 * If storage is NULL, new memory is allocated. If storage is not zero, use 
 * the memory pointed to by storage.
 * @param length the dimention of the vector
 * @param *storage point to the memory
 */
template<typename T>
inline CVector<T>::CVector(int len, T *storage)
{
	assert(len > 0);
	// TODO: make sure len and storage agrees with each other
	// if storage is not NULL, then its size has to agree with len
	_len = len;

	if(NULL == storage)
	{
		_v = new T[len];
		memset( _v, 0, _len*sizeof(T) ) ; 
	}
	else
		_v = storage;
}


/**
 * Destructor
 */
template<typename T>
inline CVector<T>::~CVector()
{
	//cout << "debug : \n" << *this << endl;
	if(_v)
	{
		delete[] _v;
		_v = NULL;
		// set _v to NULL so that the parent class won't
		// try to delete this again
	}
	//cout << "\n~CVector";
}


// ------------------------------------------------------
// Predicators
// ------------------------------------------------------
/**
 * A vector is valid if it is initialized and it's length is
 * positive
 * @return true if the vector is valid
 */
template<typename T>
inline bool CVector<T>::isValid() const
{
	return (_len > 0 && _v != NULL);
}

/**
 * Check if the vec is compatible with the current vector
 * @param vec reference to a vector
 * @return true if the vector is valid and of the same dimention
 * as the current vector, false otherwise
 */
template<typename T>
inline bool CVector<T>::isCompatible(const CVector<T>& vec) const
{
	return (isValid() && vec.isValid() && vec._len == _len);
}

// ------------------------------------------------------
// Accessors and Mutators
// ------------------------------------------------------

/**
 * p-norm of a vector
 * @param p takes three values 1, 2(default), and -1 for infinity
 * @return T p-norm
 */
template<typename T>
inline T CVector<T>::norm(char p) const
{
	assert(isValid());
	switch(p)
	{
	case '1':	// sum of the absolute value of elements
	case 1:
		return norm1();
		break;
	case '2':
	case 2:
		return norm2();
		break;
	case 'i': // infinity-norm is the absolute value of the maximal element
	case 'I':
		return normInfinity();
		break;
	default:
		cout << "Error - Vector P-norm are not defined for P = " << p << endl;
		return -1;
		break;
	}
}

/**
 * 1-norm of a vector.
 * It's the sum of the absolute value of all elements.
 * @return T 1-norm of a vector
 */
template<typename T>
inline T CVector<T>::norm1() const
{
	assert(isValid());
	T sum = 0;
	for(int i = 0; i<_len; i++)
		sum += abs(_v[i]);
	return sum;
}

/**
 * infinity-norm of a vector
 * It's the absolute value of the element with largest absolute value.
 * @return T Infinity-norm of a vector
 */
template<typename T>
inline T CVector<T>::normInfinity() const
{
	assert(isValid());
	T norm = abs(_v[0]);
	for(int i = 1; i<_len; i++)
		if(norm < abs(_v[i]))
			norm = abs(_v[i]);
	return norm;
}

/**
 * 2-norm of a vector
 * @return T 2-norm of a vector
 */
template<typename T>
inline T CVector<T>::norm2() const
{
	assert(isValid());
	T sum = 0;
	for(int i = 0; i<_len; i++)
		sum += _v[i]*_v[i];

	return sqrt(sum);
}

/**
 * 2-norm squared of a vector
 * @return T 2-norm squared of a vector
 */
template<typename T>
inline T CVector<T>::norm2Squared() const
{
	assert(isValid());
	T sum = 0;

	for(int i = 0; i<_len; i++)
		sum += _v[i]*_v[i];
	return sum;
}



/**
 * Normalize the vector \n
 * v = v/v.norm2()
 * @return a reference to the vector
 */
template<typename T>
inline CVector<T>& CVector<T>::normalize()
{
	assert(isValid());
	T magInv = T(1)/norm2();

	for(int i= 0; i<_len; i++)
		_v[i] *= magInv;
	return (*this);
}

/**
 * Returns a vector that is the normalized version of the current vector.
 * @return a vector that is the normalized version of the current vector.
 */
template<typename T>
inline CVector<T> CVector<T>::normalized() const
{
	assert(isValid());
	CVector<T> vec(*this);
	return vec.normalize();
}


/**
 * Reverse the vector \n
 * @return a reference to the vector
 */
template<typename T>
inline CVector<T>& CVector<T>::reverse()
{
	assert(isValid());

	CVector vec(*this);
	for(int i= 0; i<_len; i++)
		_v[i] = vec[_len-i-1];
	return (*this);
}

/**
 * Returns a vector that is the reversed  version of the current vector.
 * @return a vector that is the reversed version of the current vector.
 */
template<typename T>
inline CVector<T> CVector<T>::reversed() const
{
	assert(isValid());
	CVector<T> vec(*this);
	return vec.reverse();
}

// ------------------------------------------------------
// Overloaded Operators 
// ------------------------------------------------------
 
/**
 * operator [] \n
 * @param i index
 * @return the refernce to the current vecotr element
 */
template<typename T>
inline T& CVector<T>::operator[] (int i)
{
	assert(isValid());
	assert(i >= 0 && i< _len);
	
	return _v[i];
}

/**
 * operator [] (Read only) \n
 * Returns vector element value specified by index i (Read Only).
 * @param i index
 * @return the value of the current vector element
 */
template<typename T>
inline T CVector<T>::operator[] (int i) const
{
	assert(isValid());
	assert(i >= 0 && i< _len);
	
	return _v[i];
}

/**
 * operator = \n
 * Assigns value to all elements of this vector.
 * @param value value to be assigned
 * @return reference to the current vector 
 */
template<typename T>
inline CVector<T>& CVector<T>::operator= (T value) 
{
	assert(isValid());
	for(int i = 0; i<_len; i++)
		_v[i] = value;

	return (*this);
}

/**
 * operator = (const CVector& vec) \n
 * Copies the values of another vector to the 
 * corresponding elements of this vector.
 * @param vec 
 * @return reference to the current vector 
 */
template<typename T>
inline CVector<T>& CVector<T>::operator= (const CVector& vec) 
{
	assert(vec.isValid());

	// handle self-assignment
	if(this == &vec) return *this;

	if(_len == vec._len)
	{
		memcpy(_v, vec._v, _len*sizeof(T));
	}
	else
	{
		// release the memory before we copy vec over
		if(_v)
		{
			delete[] _v;
			_v = NULL;
		}
			

		_len = vec._len;
		_v = new T[_len];
		memcpy(_v, vec._v, _len*sizeof(T));
	}
	return (*this);
}	

/**
 * operator - [unary] \n
 * @return a vector in which the elements of 
 * the current vector have been multiplied by -1
 */
template<typename T>
inline CVector<T> CVector<T>::operator- (void) const 
{
	assert(isValid());
	CVector<T> result(_len);

	for(int i = 0; i<_len; i++)
		result._v[i] = -_v[i];

	return result;
}

/**
 * operator + \n
 * @param vec reference to a vector 
 * @return a vector that is the sum of the current vector and vec.
 */
template<typename T>
inline CVector<T> CVector<T>::operator+ (const CVector<T>& vec) const 
{
	assert(isCompatible(vec));
	CVector<T> result(_len);

	for(int i = 0; i<_len; i++)
		result._v[i] = _v[i] + vec._v[i];

	return result;
}

/**
 * operator - \n
 * @param vec reference to a vector  
 * @return a vector that is the difference of the current vector and vec.
 */
template<typename T>
inline CVector<T> CVector<T>::operator- (const CVector<T>& vec) const 
{
	assert(isCompatible(vec));
	CVector<T> result(_len);

	for(int i = 0; i<_len; i++)
		result._v[i] = _v[i] - vec._v[i];

	return result;
}

/**
 * operator * (dot product with refvecotr) \n
 * @param &vec reference to a vector  
 * @return the dot product of vec and current vector.
 */
template<typename T>
inline T CVector<T>::operator* (const CVector<T> &vec) const
{
	assert(isCompatible(vec));
	T dot_prod = 0;

	for(int i = 0; i<_len; i++)
		dot_prod += _v[i] * vec._v[i];

	return dot_prod;
}

/**
 * operator * (multiplication by T) \n
 * @param s T as a mulitplyer
 * @return a vector that is the current vector multiplied the s value.
 */
template<typename T>
inline CVector<T> CVector<T>::operator* (T s) const
{
	assert(isValid());
	CVector<T> result(_len);

	for(int i = 0; i<_len; i++)
		result._v[i] = _v[i] * s;
	return result;
}

/**
 * operator / (division by T)\n
 * @param s T as a divider
 * @return a vector that is the current vector divided the s value. 
 */
template<typename T>
inline CVector<T> CVector<T>::operator/ (T s) const 
{
	assert(isValid());
	CVector<T> result(_len);
	T sInv = T(1)/s;

	for(int i = 0; i<_len; i++)
		result._v[i] = _v[i] * sInv;
	return result;	
}

/**
 * operator += \n
 * Adds the elements of vec to the corresponding elements of the current vector.
 * @param vec reference to a vector  
 * @return reference to the current vector
 */
template<typename T>
inline CVector<T>& CVector<T>::operator+= (const CVector& vec) 
{
	assert(isCompatible(vec));
	for(int i=0; i < _len; i++)
		_v[i] += vec._v[i];
	return *this;
}

/**
 * operator -= \n
 * Subtracts the elements of vec from the corresponding elements of the current vector.
 * @param vec reference to a vector  
 * @return reference to the vector
 */
template<typename T>
inline CVector<T>& CVector<T>::operator-= (const CVector& vec) 
{
	assert(isCompatible(vec));
	for(int i=0; i < _len; i++)
		_v[i] -= vec._v[i];
	return *this;
}

/**
 * operator *=\n
 * Multiplies the elements of the current vector by s.
 * @param s T as a multiplyer
 * @return reference to the vector
 */
template<typename T>
inline CVector<T>& CVector<T>::operator*= (T s) 
{
	assert(isValid());
	for(int i=0; i < _len; i++)
		_v[i] *= s;
	return *this;
}

/**
 * operator /= \n
 * Divides the elements of the current vector by s.
 * @param s T as a divider
 * @return reference to the vector
 */
template<typename T>
inline CVector<T>& CVector<T>::operator/= (T s) 
{
	assert(isValid());
	T sInv = T(1)/s;
	for(int i=0; i < _len; i++)
		_v[i] *= sInv;
	return *this;
}

/**
 * operator ==\n
 * @param vec reference to a vector  
 * @return true if all elements of vec are equal 
 * to the corresponding elements of the current vector.
 */
template<typename T>
inline bool CVector<T>::operator== (const CVector<T>& vec) const 
{
	assert(isCompatible(vec));
	//for(int i=0; i < _len; i++)
	//	if(_v[i] != vec._v[i])
	//		return false;

	return !memcmp(_v, vec._v, _len*sizeof(T));
}

/**
 * operator !=\n
 * @param vec reference to a vector  
 * @return true if any element of vec is not equal 
 * to the corresponding element of the current vector.
 */
template<typename T>
inline bool CVector<T>::operator!= (const CVector<T>& vec) const 
{
	assert(isCompatible(vec));
	return !(*this == vec);
}


/**
 * operator <\n
 * This operator is defined so this class can be used in 
 * STL containers.
 * @param vec reference to a vector  
 * @return true if the norm2 of the vec is smaller
 * than the current one.
 */
template<typename T>
inline bool CVector<T>::operator< (const CVector<T>& vec) const 
{
	assert(isCompatible(vec));
	return (norm2Squared() <  vec.norm2Squared());
}

/**
* Function:	lerp\n
* Purpose:		Linear Interpolation Between two Vectors\n
* Adjust the DELTA constant to play with the LERP vs. SLERP level\n
* @param v1 vector
* @param v2 vector
* @param blend blend factor
* @return result vector which is a linear interpolation between two input vectors
*/
template<typename T>
inline CVector<T> lerp(const CVector<T> &v1, const CVector<T> &v2, T blend)
{
	// make sure that blend is in [0, 1]
	assert(blend >= 0 && blend <= 1);
	assert(v1.isCompatible(v2));
	return v1*(1-blend) + v2 *(blend);
}

// this is needed since we wanna be able to write s * vec
template<typename T> 
inline int operator*(T s, const CVector<T>& vec)
{
	assert(vec.isValid());
	return vec*s;
}





// ==================================================================================
/**
 * @class CVector3 Vector.h Vector.h
 * @brief 3-Dimensional Vector class
 *
 * @ingroup Lib
 *
 * @par requirements
 * MFC\n
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
 * @date 2003-02-23
 * @author howardz
 *
 *
 * @todo none
 * @bug none
 * @warning none
 */
// ================================================================================
template<typename T>
class CVector3: public CVector<T>
{
	using CVector<T>::_v;
	using CVector<T>::_len;
public:
	// ------------------------------------------------------
	// constructors and destructors
	// ------------------------------------------------------
	/// default constructor
	CVector3 (void):
	  CVector<T>(3){}

	CVector3 (const CVector3<T> &vec);
	CVector3 (const CVector<T>  &vec);
	CVector3 (T x, T y, T z);

	virtual ~CVector3()
	{
		if(_v)
		{
			delete[] _v;
			_v = NULL;
		}
		//cout << "\n~CVector 3";
	}

	virtual bool isValid() const
	{
		return (_len == 3 && _v != NULL);
	}

	// ------------------------------------------------------
	// Accessors and Mutators
	// ------------------------------------------------------
	void set (T x, T y, T z);
	CVector3 cross(const CVector3<T> &vec) const;

	// ------------------------------------------------------
	// Output or Test
	// ------------------------------------------------------
	static void test(void);

	/**
	* Coordinate system from a vector
	* from "Physically Based Image Synthesis" by Mat Pharr and Greg Humphreys
	*
	* We can use the fact that the cross product gives a vector orthogonal to the two
	* vectors to write a function that takes one vector and returns two new vectors so that
	* the three of them form an orthonormal coordiante system. Specifically, all three
	* of the vectors will be perpendicular to each other. Note that the other two vectors
	* returned are only unique up to a rotation about the given vector. This function
	* assumes that the vector passed in, v1, has already been normalized.
	* We first construct a perpendicular vector by zeroing one of the two components
	* of the original vector and permuting the remaining two. Inspection of the two cases
	* should make clear that v2 will be normalized and that the dot product of v1 and v2 will
	* be equal to zero. Given these two perpendicular vectors, one more cross product
	* wraps things up to give us the third, which by definition of the cross product will
	* be be perpendicular to the first two.
	**/
	friend void coordinateSystem(const CVector3<T> &v1, CVector3<T> &v2, CVector3<T> &v3) 
	{
		if (fabs(v1._v[0]) > fabs(v1._v[1])) 
		{
			T invLen = T(1)/sqrt(v1._v[0]*v1._v[0] + v1._v[2]*v1._v[2]);
			v2 = CVector3<T>(-v1._v[2] * invLen, 0, v1._v[0] * invLen);
		}
		else 
		{
			T invLen = T(1)/ sqrt(v1._v[1]*v1._v[1] + v1._v[2]*v1._v[2]);
			v2 = CVector3<T>(0, v1._v[2] * invLen, -v1._v[1] * invLen);
		}
		v3 = v1.cross(v2);
	}

	/**
	 * Cross product \n
	 * @param &vec reference to a vector
	 * @return the cross product between the two vectors
	 */
	friend CVector3<T> cross(const CVector3<T> &v1, const CVector3<T> &v2)
	{
		return CVector3<T>(	v1._v[1]*v2._v[2] - v1._v[2]*v2._v[1],
							v1._v[2]*v2._v[0] - v1._v[0]*v2._v[2],
							v1._v[0]*v2._v[1] - v1._v[1]*v2._v[0]);
	}
};



// ======================================================
// Actual function definition for CVector3
// ======================================================

/**
 * Constructor \n
 * @param x 
 * @param y 
 * @param z 
 */
template<typename T>
inline CVector3<T>::CVector3(T x, T y, T z):CVector<T>(3)
{
	_v[0] = x;
	_v[1] = y;
	_v[2] = z;
}

/**
 * Copy Constructor \n
 * @param &vec reference to a vector 
 */
template<typename T>
inline CVector3<T>::CVector3 (const CVector3<T> &vec)
{
	assert(&vec != this);
	assert(vec._v != NULL);

	_len = 3;
	_v = new T[3];
	_v[0] = vec._v[0];
	_v[1] = vec._v[1];
	_v[2] = vec._v[2];
}
		
/**
 * constructor (type casting)\n
 * @param &vec reference to a vector
 */
template<typename T>
inline CVector3<T>::CVector3 (const CVector<T> &vec)
{
	assert(vec.isValid());
	assert(&vec != this);
	assert(3 == vec.length());

	_len = 3;
	_v = new T[3];

	_v[0] = vec[0];
	_v[1] = vec[1];
	_v[2] = vec[2];
}

/**
 * Sets the value of the current vector
 * @param x 
 * @param y 
 * @param z 
 */
template<typename T>
inline void CVector3<T>::set(T x, T y, T z)
{
	_v[0] = x; _v[1] = y; _v[2] = z;
}

/**
 * Cross product \n
 * @param &vec reference to a vector
 * @return the cross product between the two vectors
 */
template<typename T>
inline CVector3<T> CVector3<T>::cross(const CVector3<T> &vec) const
{
	return CVector3<T>(	_v[1]*vec._v[2] - _v[2]*vec._v[1],
						_v[2]*vec._v[0] - _v[0]*vec._v[2],
						_v[0]*vec._v[1] - _v[1]*vec._v[0]);
}
// ------------------------------------------------------
// Output or Test
// ------------------------------------------------------

// ==================================================================================
/**
 * @class CVector2 Vector.h Vector.h
 * @brief 2-Dimensional Vector class
 *
 * @ingroup Lib
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
 * @date 2003-02-23
 * @author howardz
 *
 *
 * @todo none
 * @bug none
 * @warning none
 */
// ================================================================================
template<typename T>
class CVector2: public CVector<T>
{
	using CVector<T>::_v;
	using CVector<T>::_len;
public:
	// ------------------------------------------------------
	// constructors and destructors
	// ------------------------------------------------------
	/// Default Constructor
	CVector2 (void):
	  CVector<T>(2){}

	CVector2 (const CVector2<T> &vec);
	//CVector2<T>& operator=(const CVector2<T> &vec);

	CVector2 (const CVector<T>  &vec);
	CVector2 (T x, T y);

	virtual ~CVector2()
	{
		if(_v)
		{
			delete[] _v;
			_v = NULL;
		}
		//cout << "\n~CVector 2";
	}

	virtual bool isValid() const
	{
		return (_len == 2 && _v != NULL);
	}
	// ------------------------------------------------------
	// Accessors and Mutators
	// ------------------------------------------------------
	void set (T x, T y);

	// ------------------------------------------------------
	// Output or Test
	// ------------------------------------------------------
	static void test();
};


/**
 * Constructor \n
 * @param x 
 * @param y 
 * @param z 
 * @param w 
 */
template<typename T>
inline CVector2<T>::CVector2(T x, T y):CVector<T>(2)
{
	_v[0] = x;
	_v[1] = y;
}

/**
 * Copy Constructor \n
 * @param &vec reference to a vector
 */
template<typename T>
inline CVector2<T>::CVector2 (const CVector2<T> &vec)
{
	assert(&vec != this);
	assert(vec._v != NULL);

	_len = 2;
	_v = new T[2];
}

/**
 * constructor (type casting CVector into CVector4)\n
 * @param &vec reference to a vector
 */
template<typename T>
inline CVector2<T>:: CVector2 (const CVector<T> &vec)
{
	assert(vec.isValid());
	assert(&vec != this);
	assert(2 == vec.length());

	_len = 2;
	_v = new T[2];

	_v[0] = vec._v[0];
	_v[1] = vec._v[1];
}

/**
 * Sets the value of the current vector
 * @param x 
 * @param y 
 * @param z 
 * @param w 
 */
template<typename T>
inline void CVector2<T>::set(T x, T y)
{
	_v[0] = x; _v[1] = y;
}


// ==================================================================================
/**
 * @class CVector4 Vector.h Vector.h
 * @brief 4-Dimensional Vector class
 *
 * @ingroup Lib
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
 * @date 2003-02-23
 * @author howardz
 *
 *
 * @todo none
 * @bug none
 * @warning none
 */
// ================================================================================
template<typename T>
class CVector4: public CVector<T>
{
	using CVector<T>::_v;
	using CVector<T>::_len;
public:
	// ------------------------------------------------------
	// constructors and destructors
	// ------------------------------------------------------
	/// Default Constructor
	CVector4 (void):
	  CVector<T>(4){}

	CVector4 (const CVector4<T> &vec);
	//CVector4<T>& operator=(const CVector4<T> &vec);

	CVector4 (const CVector<T>  &vec);
	CVector4 (T x, T y, T z, T w);

	virtual ~CVector4()
	{
		if(_v)
		{
			delete[] _v;
			_v = NULL;
		}
		//cout << "\n~CVector 4";
	}

	virtual bool isValid() const
	{
		return (_len == 4 && _v != NULL);
	}
	// ------------------------------------------------------
	// Accessors and Mutators
	// ------------------------------------------------------
	void set (T x, T y, T z, T w);

	// ------------------------------------------------------
	// Output or Test
	// ------------------------------------------------------
	static void test();
};


/**
 * Constructor \n
 * @param x 
 * @param y 
 * @param z 
 * @param w 
 */
template<typename T>
inline CVector4<T>::CVector4(T x, T y, T z, T w):CVector<T>(4)
{
	_v[0] = x;
	_v[1] = y;
	_v[2] = z;
	_v[3] = w;
}

/**
 * Copy Constructor \n
 * @param &vec reference to a vector
 */
template<typename T>
inline CVector4<T>::CVector4 (const CVector4<T> &vec)
{
	assert(&vec != this);
	assert(vec._v != NULL);

	_len = 4;
	_v = new T[4];
	_v[0] = vec._v[0];
	_v[1] = vec._v[1];
	_v[2] = vec._v[2];
	_v[3] = vec._v[3];
}

/**
 * constructor (type casting CVector into CVector4)\n
 * @param &vec reference to a vector
 */
template<typename T>
inline CVector4<T>:: CVector4 (const CVector<T> &vec)
{
	assert(vec.isValid());
	assert(&vec != this);
	assert(4 == vec.length());

	_len = 4;
	_v = new T[4];

	_v[0] = vec._v[0];
	_v[1] = vec._v[1];
	_v[2] = vec._v[2];
	_v[3] = vec._v[3];
}

/**
 * Sets the value of the current vector
 * @param x 
 * @param y 
 * @param z 
 * @param w 
 */
template<typename T>
inline void CVector4<T>::set(T x, T y, T z, T w)
{
	_v[0] = x; _v[1] = y; _v[2] = z; _v[3] = w;
}

// ------------------------------------------------------
// Output or Test
// ------------------------------------------------------
/**
 * General test function for CVector class
 * put anything you want to test here.
 */
template<typename T>
void CVector<T>::test()
{
	cout << "\n\nCVector Test\n=========================\n";

	CVector<T> v1(4, 1);
	cout << endl << v1;
	CVector<T> v2(4, 2);
	cout << endl << v2;

	v1 = v2;
	cout << endl << v1;
	v2 = 3;
	v2[1] = -2;
	cout << endl << v2;
	v1 = -v2;
	cout << endl << v1;
	cout << endl << v2;
	printf("\nlength %d, norm2 %f\n", v2.length(), v2.norm('2'));
	printf("\nlength %d, norm1 %f\n", v2.length(), v2.norm('1'));
	printf("\nlength %d, normInfinity %f\n", v2.length(), v2.norm('i'));
	v2.normalize();
	cout << endl << v2;

	//CVector<T> v3(4, 5);
	//v3[1] = 0;
	//v3[2] = 0;
	//v3[3] = 12;
	//cout << endl << v3;
	//printf("\nlength %d, norm2 %f\n", v3.length(), v3.norm2());
	//v3.normalize();
	//printf("\nlength %d, norm2 %f\n", v3.length(), v3.norm2());
	//cout << endl << v3;
	//CVector<T> v31(v3.normalized());
	//printf("\nlength %d, norm2 %f\n", v31.length(), v31.norm2());
	//cout << "\nv3  : " << v3;
	//cout << "\nv31 : " << v31;

	//if(v3 != v31)
	//	cout << "\nnot equal\n";
	//else
	//	cout << "\nequal\n";

	//v3[0] = 0;
	//v3[3] = 0;
	//v3[2] = 1;
	//printf("\nlength %d, norm2 %f\n", v3.length(), v3.norm2());
	//v3.normalize();
	//printf("\nlength %d, norm2 %f\n", v3.length(), v3.norm2());
	//cout << endl << v3;	
	//
	//v3[0] = 5;
	//v3[1] = 0;
	//v3[2] = 6;
	//v3[3] = 12;

	//cout << v3 << std::endl;

	//printf("+, +=, -, -=, *, *=, /, /=\n");
	//v1= CVector<T>(4, 1);
	//cout << v1 << std::endl;
	//v2= CVector<T>(4);
	//cout << v2 << std::endl;

	//CVector<T> v4 = v1+v2;
	//cout << v4 << std::endl;
	//v4 += v1;
	//cout << v4 << std::endl;
	//v4 = v1 - v2;
	//cout << v4 << std::endl;
	//v4 -= v1;
	//cout << v4 << std::endl;

	//cout << v3 << std::endl;
	//cout << "v3 * v3 = " <<  v3 * v3 << endl;
	//v4 = CVector<T>(4, 1);
	//cout << v4 << std::endl;
	//v4 *= 2;
	//cout << v4 << std::endl;
	//v4 = v4 - v1;
	//cout << v4 << std::endl;
	//v4 -= v3;
	//cout << v4 << std::endl;
	//cout << v2 << std::endl;

	//if(v2 == v4)
	//	cout << "\nequal\n";
	//else
	//	cout << "\nnot equal\n";
}



/**
 * General test function for CMatrix class
 * put anything you want to test here.
 */
template<typename T>
void CVector3<T>::test()
{
	cout << "\n\nCVector3 Test\n=========================\n";
	CVector3<T> v1(1, 2, 3);
	CVector3<T> v2(4, 5, 6);
	CVector3<T> v3 = v1.cross(v2);
	cout << v3;

	v3 = v2.cross(v1);
	cout << v3;

	cout << "\n Test coordinate system -------------------\n";
	v1 = CVector3<T>(1, 0, 0);
	coordinateSystem(v1, v2, v3);
	cout << v1 << endl;
	cout << v2 << endl;
	cout << v3 << endl;
}


/**
 * General test function for CVector4 class
 * put anything you want to test here.
 */
template<typename T>
void CVector4<T>::test()
{
	cout << "\n\nCVector4 Test\n=========================\n";
}




// test case output
// ===================================================
//CVector Test
//=========================
//
//[    1.000    1.000    1.000    1.000 ]
//[    2.000    2.000    2.000    2.000 ]
//[    2.000    2.000    2.000    2.000 ]
//[    3.000   -2.000    3.000    3.000 ]
//[   -3.000    2.000   -3.000   -3.000 ]
//[    3.000   -2.000    3.000    3.000 ]
//length 4, norm2 5.567764
//
//[    0.539   -0.359    0.539    0.539 ]
//[    5.000    0.000    0.000   12.000 ]
//length 4, norm2 13.000000
//
//length 4, norm2 1.000000
//
//[    0.385    0.000    0.000    0.923 ]
//length 4, norm2 1.000000
//
//v3  : [    0.385    0.000    0.000    0.923 ]
//v31 : [    0.385    0.000    0.000    0.923 ]
//equal
//
//length 4, norm2 1.000000
//
//length 4, norm2 1.000000
//
//[    0.000    0.000    1.000    0.000 ] [    5.000    0.000    6.000   12.000 ]
//
//+, +=, -, -=, *, *=, /, /=
//[    1.000    1.000    1.000    1.000 ]
//[    0.000    0.000    0.000    0.000 ]
//[    1.000    1.000    1.000    1.000 ]
//[    2.000    2.000    2.000    2.000 ]
//[    1.000    1.000    1.000    1.000 ]
//[    0.000    0.000    0.000    0.000 ]
//[    5.000    0.000    6.000   12.000 ]
//v3 * v3 = 205.000
//[    1.000    1.000    1.000    1.000 ]
//[    2.000    2.000    2.000    2.000 ]
//[    1.000    1.000    1.000    1.000 ]
//[   -4.000    1.000   -5.000  -11.000 ]
//[    0.000    0.000    0.000    0.000 ]
//
//not equal
//
//
//CVector3 Test
//=========================
//[   -3.000    6.000   -3.000 ] [    3.000   -6.000    3.000 ]
// Test coordinate system -------------------
//[    1.000    0.000    0.000 ]
//[    0.000    0.000    1.000 ]
//[    0.000   -1.000    0.000 ]


#endif
