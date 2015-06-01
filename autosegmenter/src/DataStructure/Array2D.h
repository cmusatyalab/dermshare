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
 * Data Structure\n
 * @file Array2D.h
 *
 * Primary Author: Howard Zhou (howardz@cc.gatech.edu)\n
 *
 *
 * ==================================================================\n
 * General 2D Array class
 *
 *
 * @version 1.0\n
 *     Initial Revision: Seperated from the original Matrix class
 * @date 2007-02-20
 * @author howardz
 *
 *
 * @todo none
 * @bug none
 * @warning none
 *
 * ===================================================================\n
 */

#ifndef ___ARRAY_2D__H___
#define ___ARRAY_2D__H___


#include <iostream>


/// @namespace using standard namespace
using namespace std;

// ==================================================================================
/**
 * @class CArray2D Array2D.h
 * @brief General 2D Array class
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
 *     Initial Revision: Seperated from the original Matrix class
 * @date 2007-02-20
 * @author howardz
 *
 *
 * @todo none
 * @bug none
 * @warning none
 */
// ================================================================================
template<typename T>
class CArray2D
{
protected:
	int _r;     ///< number of rows
	int _c;     ///< number of columns
	int _len;	///< total number of elements for fast traversing for all the elements
	T* _m;	    ///< T array to hold data
	T**_rowPtr;	///< row ptr to each row of this matrix

private:
	// ------------------------------------------------------
	// private helpers
	// ------------------------------------------------------

public:
	// ------------------------------------------------------
	// constructors and destructors
	// ------------------------------------------------------

	/// Default constructor. Creates an uninitialized matrix.
	CArray2D(void):
	  _r(0),_c(0),_len(0),_m(NULL),_rowPtr(NULL)
	{};
	
	CArray2D(int rows, int cols, T value);
	CArray2D(int rows, int cols, T *storage = NULL);
	
	/// Copy constructor
	CArray2D(const CArray2D<T> &ary);

	virtual ~CArray2D<T> (void);	// Destructor

	// ------------------------------------------------------
	// Predicators
	// ------------------------------------------------------
	virtual bool		isValid		(void) const;
	bool				isCompatible(const CArray2D<T>& ary) const;

	// ------------------------------------------------------
	// Accessors and Mutators
	// ------------------------------------------------------
	virtual int			rows		(void) const {return _r;};
	virtual int			cols		(void) const {return _c;};
	virtual T*			data		(void) const {return _m;};
	int					length		(void) const {return _len;};

	// ------------------------------------------------------
	// Some useful Properties
	// ------------------------------------------------------
	virtual T			sum			(void) const;
	CArray2D<T>			normalized	(void) const;

	// ------------------------------------------------------
	// Overloaded Operators 
	// ------------------------------------------------------
	T*			 operator[] (int i) const;
	CArray2D<T>  operator-  (void)  const;
	CArray2D<T>& operator=  (T value); 
	CArray2D<T>& operator=  (const  CArray2D<T>& ary);
	CArray2D<T>  operator+  (const  CArray2D<T>& ary) const;
	CArray2D<T>  operator-  (const  CArray2D<T>& ary) const;
	CArray2D<T>  operator*  (T s) const;
	CArray2D<T>  operator*  (const  CArray2D<T>& ary) const; 
	CArray2D<T>  operator/  (T s) const;
	CArray2D<T>& operator+= (const  CArray2D<T>& ary);
	CArray2D<T>& operator-= (const  CArray2D<T>& ary);
	CArray2D<T>& operator*= (T s);
	CArray2D<T>& operator*= (const  CArray2D<T>& ary);
	CArray2D<T>& operator/= (T s);
	bool		 operator== (const  CArray2D<T>& ary) const;
	bool		 operator!= (const  CArray2D<T>& ary) const;
	bool		 operator<  (const  CArray2D<T>& ary) const;


	// ------------------------------------------------------
	// Output or Test
	// ------------------------------------------------------
	static void test(void);

	// ------------------------------------------------------
	// Friend functions
	// ------------------------------------------------------
	/**
	* operator <<:
	* Display matrix through the ostream
	* @param os reference to the ostream object
	* @param ary reference to a matrix 
	* @return ostream with the new stream of ary contents attached
	*/
	friend ostream& operator<< (ostream& os, const CArray2D<T>& ary)
	{
		assert(ary.isValid());
		for(int i=0; i< ary._r; i++)
		{
			os << "| ";
			for(int j= 0; j<ary._c; j++)
			{	
				os << setprecision(5);
				os << setw(10);
				os << setiosflags (ios_base::fixed | ios_base::right);
				os << ary[i][j] << "  ";
			}
			os << "|\n";
		}
		return os;
	}

	/**
	* operator *
	* @param s T-precision multiplier or divider
	* @return a matrix that is the product of the current matrix and s.
	*/
	friend inline CArray2D<T> operator* (T s, const CArray2D<T> & ary)
	{
		assert(ary.isValid());
		CArray2D<T> result(ary);

		for(int i = 0; i< ary._len; i++)
			result._m[i] *= s;

		return result;	
	}
};


//template<typename T> bool QRDecomposition(const CArray2D<T> &ary, CArray2D<T> &Q, CArray2D &R);

// =======================================================================
// utility functions
// =======================================================================
template<typename T> ostream& operator<< (ostream& os, const CArray2D<T>& ary);
template<typename T> CArray2D<T> operator* (T s, const CArray2D<T> & ary);


// ======================================================
// Actual function definition
// ======================================================

// ------------------------------------------------------
// constructors and destructors
// ------------------------------------------------------


/**
 * Copy Constructor\n
 * Creates a 2D array with the same properties and values as another array.
 * Argument Description 
 * @param &ary reference to another matrix 
 */
template<typename T>
inline CArray2D<T>::CArray2D(const CArray2D<T> &ary)
{
	assert(ary.isValid());

	_r = ary._r;
	_c = ary._c;
	_len = ary._len;

	_m = new T[_len];
	_rowPtr = new T* [_r];
	
	// allocate for the row pointers
	for(int i = 0; i < _r; i++)
		_rowPtr[i] = &_m[i*_c];
	memcpy(_m, ary._m, _len*sizeof(T));
}

/**
 * Constructor\n
 * If storage is zero, create a dynamically sized T-precision 
 * matrix with the number of rows and cols specified. If storage is 
 * not zero, use the memory it points to.
 * @param rows number of rows
 * @param cols number of cols
 * @param *storage
 */
template<typename T>
inline CArray2D<T>::CArray2D(int rows, int cols, T *storage):
	_r(rows),_c(cols),_len(rows*cols)
{
	if(NULL == storage)
	{
	    _m = new T[_len] ;
		_rowPtr = new T* [_r];
        memset( _m, 0, _len * sizeof(T) ) ; 
	}
	else
	{
		// the user of this function has to make sure the
		// storage space allocated for storage agrees with
		// the dimension of the matrix
		_m = storage;
		_rowPtr = new T* [_r];
	}
	// allocate for the row pointers
	for(int i = 0; i < _r; i++)
		_rowPtr[i] = &_m[i*_c];
}


template<typename T>
inline CArray2D<T>::CArray2D(int rows, int cols, T value):
	_r(rows),_c(cols),_len(rows*cols)
{
	_m = new T[_len] ;
	_rowPtr = new T* [_r];
   	// allocate for the row pointers
	for(int i = 0; i < _r; i++)
		_rowPtr[i] = &_m[i*_c];
	
	for(int i = 0; i < _len; i++)
		_m[i] = value;
}

/**
 * Destructor\n
 * clear the memory
 * @return
 */
template<typename T>
inline CArray2D<T>::~CArray2D(void) 
{
	//cout << "debug : \n" << *this << endl;
	if(NULL != _rowPtr)
	{
		delete[] _rowPtr;
		_rowPtr = NULL;
	}
	if(NULL != _m)
	{
		delete[] _m;
		_m = NULL;
	}
}


// ------------------------------------------------------
// Accessors and Mutators
// ------------------------------------------------------

// ------------------------------------------------------
// Predicators
// ------------------------------------------------------
/**
 * Check whether the current array is valid
 * @return bool: true if the current array is valid and false otherwise
 */
template<typename T>
inline bool CArray2D<T>::isValid() const
{
	return ((_r > 0 || _c > 0) && NULL != _m && NULL != _rowPtr);
}

/**
 * Check whether the array is compatible with the reference array
 * by "compatible", we mean both arrays are of the same dimention
 * same number of rows and columns, respectively.
 *
 * @param ary the reference array
 * @return bool: true if the array is compatible with "ary" 
 */
template<typename T>
inline bool CArray2D<T>::isCompatible(const CArray2D<T>& ary) const
{
	return (isValid() && ary.isValid() && 
			_r == ary._r && _c == ary._c);
}

// ------------------------------------------------------
// Overloaded Operators 
// ------------------------------------------------------

/**
 * operator []: row access
 * @param i the index of the row starting from 0 
 * @return return the address pointing to the start of the row.
 * then m[i][j] will find the specific j element in that row
 */
template<typename T>
inline T* CArray2D<T>::operator[] (int i) const 
{
	assert((i >= 0 && i< _r)	// check if the query index is out of bound.
		&& isValid());			// check if the current array is valid.
	// return the address pointing to the start of the row.
	// then m[i][j] will find the specific j element in that row
	//return (&(_m[i*_c]));
	
	// NOTE: with _rowPtr, we can save the one multiplication
	// for the row acces
	return _rowPtr[i];
}

/**
 * operator =: assignment operator
 * Assigns value to all elements of this matrix.
 * @param value the value to assign
 * @return a reference to this
 */
template<typename T>
inline CArray2D<T>& CArray2D<T>::operator= (T value) 
{
	assert(isValid());

	for(int i = 0; i<_len; i++)
		_m[i] = value;
	return (*this);
}

/**
 * operator =: assignment operator
 * Assigns the elements in another matrix to this matrix. 
 * If the number of columns or the number of rows in the 
 * matrixes are not the same, an assertion error occurs.
 * @param &ary reference to matrix whose elements are to be assigned to elements in this matrix
 * @return 
 */
template<typename T>
inline CArray2D<T>& CArray2D<T>::operator= (const CArray2D<T> &ary) 
{
	// handle self-assignment
	if(this == &ary) return *this;
	if(isCompatible(ary))
	{
		// no memory reallocation necessary
		memcpy(_m, ary._m, _len*sizeof(T));
	}
	else
	{
		// need to clear the previous memory contents and
		// reallocate memory

		// clear the old contents
		if(NULL != _rowPtr)
		{
			delete[] _rowPtr;
			_rowPtr = NULL;
		}
		if(NULL != _m)
		{
			delete[] _m;
			_m = NULL;
		}

		_r = ary._r;
		_c = ary._c;
		_len = ary._len;
		_m = new T[_len];
		_rowPtr = new T* [_r];
		// allocate for the row pointers
		for(int i = 0; i < _r; i++)
			_rowPtr[i] = &_m[i*_c];

		memcpy(_m, ary._m, _len*sizeof(T));

	}
	return (*this);
}

/**
 * operator - [unary]
 * @return an array in which the elements of the current array have been multiplied by -1.
 */
template<typename T>
inline CArray2D<T> CArray2D<T>::operator- (void) const 
{
	assert(isValid());
	CArray2D<T> ary(*this);
	for(int i = 0; i<_len; i++)
		ary._m[i] = -ary._m[i];
	return ary;
}


/**
 * operator +
 * @param ary the reference array to add
 * @return an array that is the sum of the current array and ary.
 */
template<typename T>
inline CArray2D<T> CArray2D<T>::operator+ (const CArray2D<T>& ary) const 
{
	assert(isCompatible(ary));
	CArray2D<T> result(*this);
	for(int i = 0; i<_len; i++)
		result._m[i] += ary._m[i];
	return result;	
}

/**
 * operator -
 * @param ary the reference array to subtract from the current array
 * @return an array that is the difference of the current array and ary.
 */
template<typename T>
inline CArray2D<T> CArray2D<T>::operator- (const CArray2D<T>& ary) const 
{
	assert(isCompatible(ary));
	CArray2D<T> result(*this);
	for(int i = 0; i<_len; i++)
		result._m[i] -= ary._m[i];
	return result;		
}

/**
 * operator *
 * @param s T-precision multiplier or divider
 * @return a array that is the product of the current array and s.
 */
template<typename T>
inline CArray2D<T> CArray2D<T>::operator* (T s) const 
{
	assert(isValid());
	CArray2D<T> result(*this);
	for(int i = 0; i<_len; i++)
		result._m[i] *= s;
	return result;	
}

/**
 * operator /
 * @param s T-precision multiplier or divider
 * @return a array that is the quotient of the current array and s.
 */
template<typename T>
inline CArray2D<T> CArray2D<T>::operator/ (T s) const 
{
	assert(isValid());
	assert(s != 0);
	CArray2D<T> result(*this);
	T invS = T(1)/s;
	for(int i = 0; i < _len; i++)
		result._m[i] *= invS;

	return result;	
}

/**
 * operator +=
 * Adds the elements of ary to the corresponding elements of the current array.
 * @param ary reference to a array
 * @return a reference to this
 */
template<typename T>
inline CArray2D<T>& CArray2D<T>::operator+= (const CArray2D<T>& ary) 
{
	assert(isCompatible(ary));
	for(int i=0; i <_len; i++)
		_m[i] += ary._m[i];
	return (*this);	
}

/**
 * operator -=
 * Subtracts the elements of ary from the corresponding array.
 * @param ary reference to a array
 * @return a reference to this
 */
template<typename T>
inline CArray2D<T>& CArray2D<T>::operator-= (const CArray2D<T>& ary) 
{
	assert(isCompatible(ary));
	for(int i=0; i < _len; i++)
		_m[i] -= ary._m[i];
	return (*this);	
}

/**
 * operator *=
 * Multiplies the elements of the current array by s
 * @param s a T valued multiplier 
 * @return reference to this
 */
template<typename T>
inline CArray2D<T>& CArray2D<T>::operator*= (T s) 
{
	assert(isValid());
	for(int i=0; i < _len; i++)
		_m[i] *= s;
	return (*this);	
}

/**
 * operator /= 
 * Divides the elements of the current array by s.
 * @param s a T valued divider
 * @return reference to this
 */
template<typename T>
inline CArray2D<T>& CArray2D<T>::operator/= (T s) 
{
	assert(isValid());
	T invS = T(1)/s;
	for(int i=0; i < _len; i++)
		_m[i] *= invS;
	return (*this);		
}

/**
 * operator ==
 * @param ary reference to a array
 * @return true if all elements of ary are equal to the 
 * corresponding elements of the current array, or false otherwise
 */
template<typename T>
inline bool CArray2D<T>::operator== (const CArray2D<T>& ary) const 
{
	assert(isValid() && ary.isValid());
	// memcmp returns 0 if the memory contents is the same
	return (_r == ary._r && _c == ary._c // short circuit if the array dimention is different
		&& !memcmp(_m, ary._m, _len*sizeof(T)));
}

/**
 * operator !=
 * @param ary reference to a array
 * @return true if any element of ary is not equal to 
 * the corresponding element of the current array, or false otherwise
 */
template<typename T>
inline bool CArray2D<T>::operator!= (const CArray2D<T>& ary) const
{
	return !(*this == ary);
}

/**
 * operator <
 * This operator is defined so this class can be used in STL containers.
 * @param ary reference to a array
 * @return true if any element of ary is less than the corresponding
 * element of the current array. not really meaningful semantic-wise.
 */
template<typename T>
inline bool CArray2D<T>::operator< (const CArray2D<T>& ary) const 
{
	assert(isCompatible(ary));
	
	// TODO: this operation does not yet make sense right now
	for(int i = 0; i< _len; i++)
		if(_m[i] >= ary._m[i])
			return false;
	return true;
}


/**
 * Sum up all the entries of the array
 **/
template<typename T>
inline T CArray2D<T>::sum(void) const
{
	assert(isValid());
	T s = 0;
	for(int i = 0; i<_len; i++)
		s += _m[i];
	return s;
}


/**
 * Normalize the array such that all of its entries sum up to 1
 *
 **/
template<typename T>
inline CArray2D<T> CArray2D<T>::normalized(void) const
{
	assert(isValid());
	CArray2D<T> ary(*this);
	return ary/sum();
}

// ------------------------------------------------------
// Overloaded Operators 
// ------------------------------------------------------


/**
 * operator *
 * Multiply the ary with the current array. Notice that this
 * is a point-wise multiplication which only requires the two
 * arrays to be compatible (of same dimention). 
 * @param ary reference to an array
 * @return an array that is the product of the current array and ary.
 */
template<typename T>
CArray2D<T> CArray2D<T>::operator* (const CArray2D<T>& ary) const 
{
	assert(isCompatible(ary));
	CArray2D<T> result(_r, _c);
	// result is initialized so that every entry in this matrix is 0

	for(int i = 0; i < _r; i++)
		for(int j = 0;j < _c; j++)
			result[i][j] = _rowPtr[i][j] * ary[i][j];
	return result;
}

/**
 * operator *=
 * Multiplies the current arrray by ary. Notice that this
 * is a point-wise multiplication which only requires the two
 * arrays to be compatible (of same dimention). 
 * @param ary reference to an array
 * @return reference to this 
 */
template<typename T>
CArray2D<T>& CArray2D<T>::operator*= (const CArray2D<T>& ary) 
{
	assert(isCompatible(ary));
	(*this) = (*this) * ary;
	return (*this);
}




// =======================================================================
// utility functions
//
//
// =======================================================================


// ------------------------------------------------------
// Output or Test
// ------------------------------------------------------

/**
 * General test function for CArray2D<T> class
 * put anything you want to test here.
 */
template<typename T>
void CArray2D<T>::test()
{
	cout << "\n\nCArray2D Test\n=========================\n";

	CArray2D<T> m1(3, 3);
	cout << m1 << endl;

	m1[2][1] = 24;
	m1[0][2] = 15;
	m1[1][0] = -4;
	cout << m1 << endl;
	
	CArray2D<T> m2 = m1;

	cout << "equal? " << (m1 == m2) << endl;
	cout << "not equal? " << (m1 != m2) << endl;

	CArray2D<T> m4 = m1*m2;
	cout << m4 << endl;

	CArray2D<T> m5(3, 5);
	cout << "\nm5 : " << endl << m5 << endl;
	m5 = 2;
	cout << "\nm5 : " << endl << m5 << endl;

	// single column matrix
	CArray2D<T> m7(1,4);
	m7[0][0] = 1;
	m7[0][1] = 2;
	m7[0][2] = 3;
	m7[0][3] = 4;
	cout << m7 << endl;

	CArray2D<T> m8(2, 2, 1);
	CArray2D<T> m9(2, 2, 2);
	CArray2D<T> m10(2, 2, 3);
	CArray2D<T> m11(m8);
	CArray2D<T> m12(m8);
	CArray2D<T> m13(2, 2);
	m8 += m9 * m10;

	cout << m8 << endl;

	m11 = m11 + m9*m10;
	cout << m11 << endl;

	m13 = m9*m10;
	cout << m13 << endl;

	m12 = m12+m13;
	cout << m12 << endl;

	CArray2D<T> m14(4, 4);
	CArray2D<T> m15(4, 2);

	m14[0][0] = 12; m14[0][1] = 32; m14[0][2] = 1.5f; m14[0][3] = 15;
	m14[1][0] = 2.5f;m14[1][1] = 17; m14[1][2] = 26;  m14[1][3] = 9;
	m14[2][0] = 8;  m14[2][1] = 23; m14[2][2] = 19;  m14[2][3] = 3;
	m14[3][0] = 12 ;m14[3][1] = 7.2f; m14[3][2] = 2;  m14[3][3] = 10;

	m15[0][0] = 20; m15[0][1] = 15;
	m15[1][0] = 6;  m15[1][1] = 6;
	m15[2][0] = 11; m15[2][1] = 19;
	m15[3][0] = 11; m15[3][1] = 22;

	cout << "m14 : " << endl << m14 << endl;
	cout << "m15 : " << endl << m15 << endl;
}


#endif // ___ARRAY_2D__H___
