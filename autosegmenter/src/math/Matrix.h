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
 * @file Matrix.h
 *
 * Primary Author: Howard Zhou (howardz@cc.gatech.edu)\n
 *
 *
 * ==================================================================\n
 * general matrix class
 *
 * @version 1.4\n
 * @date 2007-02-20
 *     1. separated the code for 2D array structure from the matrix computation code and
 *        formed an CArray2D class.
 * @author howardz
 *
 * @version 1.3\n
 * @date 2006-10-20
 *     1. added p-norm for Vector class
 *     2. added singular value decompostion and eigenvalue decomposition
 *     3. substitute my own decomposition code with Intel MKL routines for more efficient code
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

#ifndef ___MATRIX__H___
#define ___MATRIX__H___


#include <cassert>
#include <cstdio>
#include <cmath>
#include <cstring>
#include <iostream>
#include <iomanip>

#include "DataStructure/Array2D.h"
#include "Vector.h"

// Most matrix factorization and decomposition are implemented
// efficiently in LAPACK. I decided to use the Intel MKL (Math Kernel Library)
// variant of LAPACK for my own matrix library. Only routines that we
// normaly encounter in Graphics/vision are used here.
//
// If you don't have a copy of MKL installed, just undefine USE_MKL, you 
// will still have access to some less efficient implementation of simpler
// decompositions.
# ifndef USE_MKL
# define USE_MKL 0
# endif

#ifndef EPSILON
#define EPSILON 10e-20
#endif

/// @namespace using standard namespace
using namespace std;

// ==================================================================================
/**
 * @class CMatrix<T> Matrix.h Matrix.h
 * @brief General Matrix class
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
 * @todo implement Cholesky decomp, QR decomp, and SVD decomp
 * @bug none
 * @warning none
 */
// ================================================================================
template<typename T>
class CMatrix : public CArray2D<T>
{
protected:
	// all members are defined in CArray2D
	using CArray2D<T>::_r;     ///< number of rows
	using CArray2D<T>::_c;     ///< number of columns
	using CArray2D<T>::_m;	    ///< T array to hold data
	using CArray2D<T>::_rowPtr;
	using CArray2D<T>::_len;

private:
	// ------------------------------------------------------
	// private helpers
	// ------------------------------------------------------

public:
	// ------------------------------------------------------
	// constructors and destructors
	// ------------------------------------------------------

	/// Default constructor. Creates an uninitialized matrix.
	CMatrix(void)
		:CArray2D<T>(){};
	
	CMatrix(int rows, int cols, T value)
		:CArray2D<T>(rows, cols, value){};

	CMatrix(int rows, int cols, T *storage = NULL)
		:CArray2D<T>(rows, cols, storage){};
	
	/// Copy constructor
	CMatrix(const CMatrix<T> &mat)
		:CArray2D<T>((CArray2D<T>&) mat){};

	// ------------------------------------------------------
	// Named contructors
	// ------------------------------------------------------
	static CMatrix<T> identity(int dimension);

	static CMatrix<T> diagonal(const CVector<T> vec)
	{
		assert(vec.isValid());
		CMatrix<T> m(vec.length(), vec.length());
		for(int i = 0; i < m._r; i++)
			m[i][i] = vec[i];
		return m;
	}

	static CMatrix<T> matrix2x2(T t00, T t01,
								T t10, T t11)
	{
		CMatrix<T> m(2,2);
		m[0][0] = t00; m[0][1] = t01;
		m[1][0] = t10; m[1][1] = t11;
		return m;
	}
	static CMatrix<T> matrix3x3(T t00, T t01, T t02,
								T t10, T t11, T t12,
								T t20, T t21, T t22)
	{
		CMatrix<T> m(3, 3);
		m[0][0] = t00; m[0][1] = t01; m[0][2] = t02;
		m[1][0] = t10; m[1][1] = t11; m[1][2] = t12;
		m[2][0] = t20; m[2][1] = t21; m[2][2] = t22;
		return m;
	}

	static CMatrix<T> matrix4x4(T t00, T t01, T t02, T t03,
								T t10, T t11, T t12, T t13,
								T t20, T t21, T t22, T t23,
								T t30, T t31, T t32, T t33)
	{
		CMatrix<T> m(4, 4);
		m[0][0] = t00; m[0][1] = t01; m[0][2] = t02; m[0][3] = t03;
		m[1][0] = t10; m[1][1] = t11; m[1][2] = t12; m[1][3] = t13;
		m[2][0] = t20; m[2][1] = t21; m[2][2] = t22; m[2][3] = t23;
		m[3][0] = t30; m[3][1] = t31; m[3][2] = t32; m[3][3] = t33;
		return m;
	}


	virtual ~CMatrix<T> (void);	// Destructor, virtual since the base class has a virtual destructor

	// ------------------------------------------------------
	// Predicators
	// ------------------------------------------------------
	virtual bool	isSquare		(void) const;
	virtual bool	isSymmetric		(void) const;
	virtual bool	isIdentity		(void) const;
	virtual	bool	isCompatible	(const CMatrix<T>& mat) const;
	virtual bool	isMultiplicable	(const CMatrix<T>& mat) const;
	virtual bool	isInvertible	(void) const;

	// ------------------------------------------------------
	// Accessors and Mutators
	// ------------------------------------------------------
	CVector<T>		rowVector		(int i) const;
	CVector<T>		colVector		(int i) const;
	T				maxElement		(void) const;

	// ------------------------------------------------------
	// Matrix properties
	// ------------------------------------------------------
	virtual T		trace			(void) const;
	virtual	T		determinant		(void) const;
	CMatrix<T>		inverse			(void) const;
	CMatrix<T>		transpose		(void) const;
	CMatrix<T>		normalized		(void) const;

	virtual T		norm			(char p = '2') const;
	virtual T		norm1			(void) const;
	virtual T		norm2			(void) const;
	virtual T		normInfinity	(void) const;
	virtual T		normFrobenius	(void) const;

	// ------------------------------------------------------
	// Overloaded Operators 
	// ------------------------------------------------------
	CMatrix<T>		operator-		(void)  const;
	CMatrix<T>&		operator=		(T value); 
	CMatrix<T>&		operator=		(const  CMatrix<T>& mat);
	CMatrix<T>		operator+		(const  CMatrix<T>& mat) const;
	CMatrix<T>		operator-		(const  CMatrix<T>& mat) const;
	CMatrix<T>		operator*		(T s) const;
	CMatrix<T>		operator*		(const  CMatrix<T>& mat) const; 
	CMatrix<T>		operator/		(T s) const;
	CMatrix<T>&		operator+=		(const  CMatrix<T>& mat);
	CMatrix<T>&		operator-=		(const  CMatrix<T>& mat);
	CMatrix<T>&		operator*=		(T s);
	CMatrix<T>&		operator*=		(const  CMatrix<T>& mat);
	CMatrix<T>&		operator/=		(T s);
	bool			operator==		(const  CMatrix<T>& mat) const;
	bool			operator!=		(const  CMatrix<T>& mat) const;
	bool			operator<		(const  CMatrix<T>& mat) const;

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
	* @param mat reference to a matrix 
	* @return ostream with the new stream of mat contents attached
	*/
	friend ostream& operator<< (ostream& os, const CMatrix<T>& mat)
	{
		assert(mat.isValid());
		//for(int i=0; i< mat._r; i++)
		//{
		//	os << "| ";
		//	for(int j= 0; j<mat._c; j++)
		//	{	
		//		os << setprecision(5);
		//		os << setw(10);
		//		os << setiosflags (ios_base::fixed | ios_base::right);
		//		os << mat[i][j] << "  ";
		//	}
		//	os << "|\n";
		//}
		return (os << (CArray2D<T> &)mat);
	}

	/**
	* operator *
	* @param s T-precision multiplier or divider
	* @return a matrix that is the product of the current matrix and s.
	*/
	friend inline CMatrix<T> operator* (T s, const CMatrix<T> & mat)
	{
		assert(mat.isValid());
		CMatrix<T> result(mat);

		for(int i = 0; i<_len; i++)
			result._m[i] *= s;

		return result;	
	}

	friend inline CVector<T> operator* (const CVector<T> &v, const CMatrix<T> &M)
	{
		assert(v.isValid() && M.isValid());
		assert(v.length() == M.rows());

		CVector<T> vr(M.cols(), T(0));
		for(int c = 0; c < M.cols(); c++)
			for(int r = 0; r < M.rows(); r++)
				vr[c] += v[r] * M[r][c];
		return vr;
	}

	friend inline CVector<T> operator* ( const CMatrix<T> &M, const CVector<T> &v)
	{
		assert(M.isValid() && v.isValid());
		assert(M.cols() == v.length());

		CVector<T> vr(M.rows(), T(0));
		for(int r = 0; r < M.rows(); r++)
			for(int c = 0; c < M.cols(); c++)
				vr[r] += v[c] * M[r][c];
		return vr;
	}
};

// =======================================================================
// utility functions
// =======================================================================
template<typename T> ostream&	operator<<				(ostream& os, const CMatrix<T>& mat);
template<typename T> CMatrix<T> operator*				(T s, const CMatrix<T> & mat);
template<typename T> CVector<T> operator*				(const CVector<T>& vec, const CMatrix<T>& mat);
template<typename T> CVector<T> operator*				(const CMatrix<T>& mat, const CVector<T>& vec);
template<typename T> bool		GaussJordanElimination	(CMatrix<T> &mat, CMatrix<T> &rightHandSide);
template<typename T> bool		LUDecomposition			(CMatrix<T> &mat, CVector<T> &index, int &d);
template<typename T> void		LUBacksubstitution		(const CMatrix<T> &LU, CMatrix<T> &RHS, const CVector<T> &index);

template<typename T> CVector<T> pack					(const CMatrix<T> &mat);// for symmetric matrix, store in packed format
template<typename T> CMatrix<T> unpack					(const CVector<T> &vec);
template<typename T> CVector<T> eigenValues2x2			(const CMatrix<T> &mat);
template<typename T> CVector<T> singularValues2			(const CMatrix<T> &mat);

template<typename T> CVector<T> convertToVectorColumnMajor	(const CMatrix<T> &mat);
template<typename T> void		convertToArrayColumnMajor	(const CMatrix<T> &mat, T* array);
template<typename T> void		convertToArrayRowMajor		(const CMatrix<T> &mat, T* array);


// ------------------------------------------------------
// Eigenvector, Singular value decomposition, and other factorization
// ------------------------------------------------------
// Eigenvalue decomposition
template<typename T> bool EVD(const CMatrix<T> &mat, CVector<T> &E, CMatrix<T> &V,	
							  bool bStoreEigenVectors);

// Singular Value Decomposition
template<typename T> bool SVD(const CMatrix<T> &mat, CMatrix<T> &U, CVector<T> &S, CMatrix<T> &V, bool bStoreU, bool bStoreV);


// ======================================================
// Function definition
// ======================================================

// ------------------------------------------------------
// constructors and destructors
// ------------------------------------------------------
/**
 * Copy Constructor\n
 * Creates a matrix with the same properties and values as another matrix.
 * Argument Description 
 * @param &mat reference to another matrix 
 */
//template<typename T>
//inline CMatrix<T>::CMatrix(const CMatrix<T> &mat)
//{
//	assert(this != &mat);
	//assert(mat.isValid());

	//_r = mat._r;
	//_c = mat._c;

	//_m = new T[_len];
	//_rowPtr = new T* [_r];
	//
	//// allocate for the row pointers
	//for(int i = 0; i < _r; i++)
	//	_rowPtr[i] = &_m[i*_c];
	//memcpy(_m, mat._m, _len*sizeof(T));
//}


/**
 * Constructor\n
 * If storage is zero, create a dynamically sized T-precision 
 * matrix with the number of rows and cols specified. If storage is 
 * not zero, use the memory it points to.
 * @param rows number of rows
 * @param cols number of cols
 * @param *storage
 */
//template<typename T>
//inline CMatrix<T>::CMatrix(int rows, int cols, T *storage):
//	_r(rows),_c(cols)
//{
//	if(NULL == storage)
//	{
//	    _m = new T[_len] ;
//		_rowPtr = new T* [_r];
//        memset( _m, 0, _len * sizeof(T) ) ; 
//	}
//	else
//	{
//		// the user of this function has to make sure the
//		// storage space allocated for storage agrees with
//		// the dimension of the matrix
//		_m = storage;
//		_rowPtr = new T* [_r];
//	}
//	// allocate for the row pointers
//	for(int i = 0; i < _r; i++)
//		_rowPtr[i] = &_m[i*_c];
//}

//template<typename T>
//inline CMatrix<T>::CMatrix(int rows, int cols, T value):
//	_r(rows),_c(cols)
//{
//	_m = new T[_len] ;
//	_rowPtr = new T* [_r];
//   	// allocate for the row pointers
//	for(int i = 0; i < _r; i++)
//		_rowPtr[i] = &_m[i*_c];
//	
//	for(int i = 0; i < _len; i++)
//		_m[i] = value;
//}

/**
 * Destructor\n
 * clear the memory
 * @return
 */
template<typename T>
inline CMatrix<T>::~CMatrix(void) 
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
///**
// * Check whether the current matrix is valid
// * @return bool: true if the current matrix is valid and false otherwise
// */
//template<typename T>
//inline bool CMatrix<T>::isValid() const
//{
//	return (_r > 0 || _c >0 && NULL != _m && NULL != _rowPtr);
//}

/**
 * Check whether the current matrix is a square matrix
 * A square matrix has the same number of rows and columns
 *
 * @return bool: true if the current matrix is a square matrix and false otherwise
 */
template<typename T>
inline bool CMatrix<T>::isSquare() const
{	return (_r == _c);		
}


/**
 * Check whether the current matrix is a invertible(non-singular)
 * only square matrices are invertible
 * @return bool: true if the current matrix is invertible(non-singular) and false otherwise
 */
template<typename T>
inline bool CMatrix<T>::isInvertible(void) const
{	if(!isSquare())
		return false;

	// TODO: calculation for det usually returns a very small
	// number instead of zero if the matrix is singular
	// this is due to round off error
	//if(determinant() != 0)
	if(determinant() < EPSILON)
		return true;
	return false;
}

/**
 * Returns true if this matrix is symmetric, false otherwise.
 * A symmetric matrix has the same number of rows and columns (square). 
 * The value of element (i, j) must be the same as element (j, i) for all i and j.
 *
 * @return bool: true if the matrix is symmetric
 */
template<typename T>
inline bool CMatrix<T>::isSymmetric () const 
{
	if(!isSquare())
		return false;

	for(int i=0; i< _r; i++)
		for(int j=i; j< _c; j++)
			if(_rowPtr[i][j] != _rowPtr[j][i])
				return false;
	return true;
}

/**
 * Returns true if this matrix is the identity matrix, false otherwise.
 * An identity matrix is symmtric matrix, and it has 1's on its diagonal and 0's everywhere else.
 *
 * @return bool: true if this matrix is the identity matrix, false otherwise.
 */
template<typename T>
inline bool CMatrix<T>::isIdentity () const 
{
	if(!isSquare())
		return false;

	for(int i = 0; i< _r; i++)
		if(_rowPtr[i][i] != 1)
			return false;

	for(int i = 0; i< _r; i++)
		for(int j=0; j< _c; j++)
			if(i != j)
				if(_rowPtr[i][j] != 0)
					return false;
	return true;
}

/**
 * Check whether the matrix is compatible with the reference matrix
 * by "compatible", we mean both matrices are of the same dimention
 * same number of rows and columns, respectively.
 *
 * @param mat the reference matrix
 * @return bool: true if the matrix is compatible with the mat 
 */
template<typename T>
inline bool CMatrix<T>::isCompatible(const CMatrix<T>& mat) const
{
	return (this->isValid() && mat.isValid() && 
		_r == mat._r && _c == mat._c);
}

/**
 * Check whether the matrix can be right multiplied by the reference
 * matrix. (i.e.  m.cols = mat.rows)
 *
 * @param mat the reference matrix
 * @return bool: true if the matrix can be right multiplied by the mat 
 */
template<typename T>
inline bool CMatrix<T>::isMultiplicable(const CMatrix<T>& mat) const
{
	return (this->isValid() && mat.isValid() && _c == mat._r);
}

/**
 * Make the current matrix an identity matrix
 * @return reference to this
 */
template<typename T>
inline CMatrix<T> CMatrix<T>::identity(int dimension)
{
	assert(dimension > 0);
	CMatrix<T> mat(dimension, dimension);
	for(int i=0; i< mat._r; i++)
		for(int j=0; j< mat._c; j++)
			if(i == j)
				mat[i][j] = 1;
			else
				mat[i][j] = 0;
	return mat;
}



// ------------------------------------------------------
// Overloaded Operators 
// ------------------------------------------------------

///**
// * operator []: row access
// * @param i the index of the row starting from 0 
// * @return return the address pointing to the start of the row.
// * then m[i][j] will find the specific j element in that row
// */
//template<typename T>
//inline T* CMatrix<T>::operator[] (int i) const 
//{
//	assert(isValid() && (i >= 0 && i< _r));
//	// return the address pointing to the start of the row.
//	// then m[i][j] will find the specific j element in that row
//	//return (&(_m[i*_c]));
//	
//	// NOTE:with _rowPtr, we can save the one multiplication
//	// for the row acces
//	return _rowPtr[i];
//}

/**
 * operator =: assignment operator
 * Assigns value to all elements of this matrix.
 * @param value the value to assign
 * @return a reference to this
 */
template<typename T>
inline CMatrix<T>& CMatrix<T>::operator= (T value) 
{
	assert(this->isValid());
	for(int i = 0; i<_len; i++)
		_m[i] = value;
	return (*this);

	//(CArray2D<T>&)(*this) = value; // invoke base class op= implicitly
	//return (*this);
}


/**
 * operator =: assignment operator
 * Assigns the elements in another matrix to this matrix. 
 * If the number of columns or the number of rows in the 
 * matrixes are not the same, an assertion error occurs.
 * @param &mat reference to matrix whose elements are to be assigned to elements in this matrix
 * @return 
 */
template<typename T>
inline CMatrix<T>& CMatrix<T>::operator= (const CMatrix<T> &mat) 
{
	// handle self-assignment case
	if(this == &mat) return *this;
	(CArray2D<T>&)(*this) = (CArray2D<T>&)mat;
	return (*this);
}

/**
 * operator - [unary]
 * @return a matrix in which the elements of the current matrix have been multiplied by -1.
 */
template<typename T>
inline CMatrix<T> CMatrix<T>::operator- (void) const 
{
	assert(this->isValid());
	CMatrix<T> mat(*this);
	for(int i = 0; i<_len; i++)
		mat._m[i] = -mat._m[i];
	return mat;
}


/**
 * operator +
 * @param mat the reference matrix to add
 * @return a matrix that is the sum of the current matrix and mat.
 */
template<typename T>
inline CMatrix<T> CMatrix<T>::operator+ (const CMatrix<T>& mat) const 
{
	assert(isCompatible(mat));
	CMatrix<T> result(*this);
	for(int i = 0; i<_len; i++)
		result._m[i] += mat._m[i];
	return result;	
}

/**
 * operator -
 * @param mat the reference matrix to subtract from the current matrix
 * @return a matrix that is the difference of the current matrix and mat.
 */
template<typename T>
inline CMatrix<T> CMatrix<T>::operator- (const CMatrix<T>& mat) const 
{
	
	assert(isCompatible(mat));
	CMatrix<T> result(*this);
	for(int i = 0; i<_len; i++)
		result._m[i] -= mat._m[i];
	return result;		
}

/**
 * operator *
 * @param s T-precision multiplier or divider
 * @return a matrix that is the product of the current matrix and s.
 */
template<typename T>
inline CMatrix<T> CMatrix<T>::operator* (T s) const 
{
	assert(this->isValid());
	CMatrix<T> result(*this);
	for(int i = 0; i<_len; i++)
		result._m[i] *= s;
	return result;	
}

/**
 * operator /
 * @param s T-precision multiplier or divider
 * @return a matrix that is the quotient of the current matrix and s.
 */
template<typename T>
inline CMatrix<T> CMatrix<T>::operator/ (T s) const 
{
	assert(this->isValid());
	CMatrix<T> result(*this);
	T invS = T(1)/s;
	for(int i = 0; i < _len; i++)
		result._m[i] *= invS;

	return result;	
}

/**
 * operator +=
 * Adds the elements of mat to the corresponding elements of the current matrix.
 * @param mat reference to a matrix
 * @return a reference to this
 */
template<typename T>
inline CMatrix<T>& CMatrix<T>::operator+= (const CMatrix<T>& mat) 
{
	assert(isCompatible(mat));
	for(int i=0; i < _len; i++)
		_m[i] += mat._m[i];
	return (*this);	
}

/**
 * operator -=
 * Subtracts the elements of mat from the corresponding 
 * @param mat reference to a matrix
 * @return a reference to this
 */
template<typename T>
inline CMatrix<T>& CMatrix<T>::operator-= (const CMatrix<T>& mat) 
{
	assert(isCompatible(mat));
	for(int i=0; i < _len; i++)
		_m[i] -= mat._m[i];
	return (*this);	
}

/**
 * operator *=
 * Multiplies the elements of the current matrix by s
 * @param s a T valued multiplier 
 * @return reference to this
 */
template<typename T>
inline CMatrix<T>& CMatrix<T>::operator*= (T s) 
{
	assert(this->isValid());
	for(int i=0; i < _len; i++)
		_m[i] *= s;
	return (*this);	
}

/**
 * operator /= 
 * Divides the elements of the current matrix by s.
 * @param s a T valued divider
 * @return reference to this
 */
template<typename T>
inline CMatrix<T>& CMatrix<T>::operator/= (T s) 
{
	assert(this->isValid());
	T invS = T(1)/s;
	for(int i=0; i < _len; i++)
		_m[i] *= invS;
	return (*this);		
}

/**
 * operator ==
 * @param mat reference to a matrix
 * @return true if all elements of mat are equal to the
 * corresponding elements of the current matrix, or false otherwise
 */
template<typename T>
inline bool CMatrix<T>::operator== (const CMatrix<T>& mat) const 
{
	assert(this->isValid() && mat.isValid());
	// memcmp returns 0 if the memory contents is the same
	return (_r == mat._r && _c == mat._c && !memcmp(_m, mat._m, _len*sizeof(T)));
}

/**
 * operator !=
 * @param mat reference to a matrix
 * @return true if any element of mat is not equal to 
 * the corresponding element of the current matrix, or false otherwise
 */
template<typename T>
inline bool CMatrix<T>::operator!= (const CMatrix<T>& mat) const
{
	return !(*this == mat);
}

/**
 * operator <
 * This operator is defined so this class can be used in STL containers.
 * @param mat reference to a matrix
 * @return true if any element of mat is less than the corresponding
 * element of the current matrix. not really meaningful here.
 */
template<typename T>
inline bool CMatrix<T>::operator< (const CMatrix<T>& mat) const 
{	
	assert(isCompatible(mat));
	
	// TODO: this operation does not yet make sense right now
	for(int i = 0; i< _len; i++)
		if(_m[i] >= mat._m[i])
			return false;
	return true;
}


/**
 * @return CVector<T> the row vector corresponding to the given row index.
 **/
template<typename T>
CVector<T> CMatrix<T>::rowVector(int i) const
{
	assert(this->isValid());

	T* v = new T[_c];
	memcpy(v, _rowPtr[i], _c*sizeof(T));
	CVector<T> result(_c, v);
	return result;
}
	
/**
 * @return CVector<T> the column vector corresponding to the given column index
 **/
template<typename T>
CVector<T> CMatrix<T>::colVector(int j) const
{
	assert(this->isValid());
	CVector<T> result(_r);
	for(int i = 0; i <_r; i++)
		result[i] = _rowPtr[i][j];
	return result;
}

template<typename T>
T CMatrix<T>::maxElement(void) const
{
	assert(this->isValid());
	T me = _m[0];
	for(int i = 1; i < _len; i++)
		me = (_m[i] > me)?_m[i]:me;
	return me;
}

///**
// * Sum up all the entries of the matrix
// **/
//template<typename T>
//inline T CMatrix<T>::sum(void) const
//{
//	assert(isValid());
//	T s = 0;
//	for(int i = 0; i<_len; i++)
//		s += _m[i];
//	return s;
//}

/**
 * 1-norm of a matrix
 * It's the 1-norm of the column vector with largest 1-norm
 * @return T 1-norm of a matrix
 **/
template<typename T>
inline T CMatrix<T>::norm1(void) const
{
	assert(this->isValid());
	T vectorNorm;
	T max = 0; 
	for(int j = 0; j<_c; j++)
	{
		vectorNorm = 0;
		for(int i = 0; i <_r; i++)
			vectorNorm += abs(_rowPtr[i][j]);

		if(max < vectorNorm)
			max = vectorNorm;
	}
	return max;
}

/**
 * 2-norm of a matrix
 * It's the largest singular value of the matrix
 * @return T 2-norm of a matrix
 **/
template<typename T>
inline T CMatrix<T>::norm2(void) const
{
	CVector<T> S;
	CMatrix<T> UD;
	CMatrix<T> VD;
#if USE_MKL
	if(SVD(*this, UD, S, VD, false, false) && S.isValid()) return S[0];
	else return -1;
#endif

	if(this->cols() != 2 && this->rows() != 2)
	{	cout << "\nSORRY - 2-norm of a matrix not of mx2 or 2xm dimension hasn't been implemented\n"; 
		return -1;
	}

	S = singularValues2(*this);
	if(S.isValid()) return S[0];
	else return -1;
}

/**
 * Infinity-norm of a matrix
 * It's the 1-norm of the row vector with largest 1-norm
 * @return T Infinity-norm of a matrix
 **/
template<typename T>
inline T CMatrix<T>::normInfinity(void) const
{
	assert(this->isValid());
	T vectorNorm;
	T max = 0;
	T* rowPtr = NULL;
	for(int i = 0; i<_r; i++)
	{
		vectorNorm = 0;
		rowPtr = _rowPtr[i];
		for(int j = 0; j < _c; j++)
			vectorNorm += abs(rowPtr[j]);

		if(max < vectorNorm)
			max = vectorNorm;
	}
	return max;
}

/**
 * Frobenius-norm of a matrix
 * It's the square root of the summation of all elements squared
 * @return T Frobenius-norm of a matrix
 **/
template<typename T>
inline T CMatrix<T>::normFrobenius(void) const
{
	assert(this->isValid());
	T sum = 0;
	T* rowPtr = NULL;
	for(int i = 0; i<_r; i++)
	{	rowPtr = _rowPtr[i];
		for(int j = 0; j < _c; j++)
			sum += rowPtr[j]*rowPtr[j];
	}
	return sqrt(sum);
}

/**
 * matrix norm
 * @param p takes three values 1, 2(default), 'i'('I' for infinity), and 'f'('F' for Frobenius norm)
 *  In fact, Frobenius norm is not a p-norm.
 * @return T norm of a matrix
 */
template<typename T>
inline T CMatrix<T>::norm(char p) const
{
	assert(this->isValid());
	switch(p)
	{
	case '1':	// the
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
	case 'f':
	case 'F':
		return normFrobenius();
		break;
	default:
		cout << "Error - Matrix norm is not defined for P = " << p << endl;
		return -1;
		break;
	}
}

/**
 * Normalize the matrix such that all of its entries sum up to 1
 *
 **/
template<typename T>
inline CMatrix<T> CMatrix<T>::normalized(void) const
{
	assert(this->isValid());
	return (*this)/(this->sum());
}


/**
 * Returns a matrix that is the transposition (switching of rows 
 * and columns) of the current matrix.
 * @see transpose
 * @return a new matrix that is the transpose of the current matrix
 */
template<typename T>
inline CMatrix<T> CMatrix<T>::transpose (void) const 
{
	assert(this->isValid());
	CMatrix<T> mat(_c, _r);
	for(int i = 0; i< _r; i++)
		for(int j=0; j< _c; j++)
				mat[j][i] = _rowPtr[i][j];
	return mat;
}

template<typename T> bool		LUDecomposition			(CMatrix<T> &mat, CVector<T> &index, int &d);

/**
 * Return the determinant of the matrix
 * calculate determinant using Gauss-Jordan elimination
 * @return T: the determinant of the matrix
 */
template<typename T>
inline T CMatrix<T>::determinant(void) const
{
	// determinants are defined only for square matrices
	assert(this->isValid());
	assert(isSquare() && "ERROR - determinants are defined only for square matrices");

	// A = | a  b |
	//     | c  d |
	// |A| = ad - bc
	if(_r == 2)
		return (_rowPtr[0][0]*_rowPtr[1][1] - _rowPtr[0][1]*_rowPtr[1][0]);
	// A = | a  b  c |
	//     | d  e  f |
	//     | g  h  i |
	// |A| = a(ei - fh) - b(di - fg) + c(dh - eg) 
	else if(_r == 3) 
		return (_rowPtr[0][0]*(_rowPtr[1][1]*_rowPtr[2][2] - _rowPtr[1][2]*_rowPtr[2][1]) -
				_rowPtr[0][1]*(_rowPtr[1][0]*_rowPtr[2][2] - _rowPtr[1][2]*_rowPtr[2][0]) +
				_rowPtr[0][2]*(_rowPtr[1][0]*_rowPtr[2][1] - _rowPtr[1][1]*_rowPtr[2][0]));
	else
	{
		int d;
		T det = 1;
		CVector<T> index(_r);
		CMatrix<T> mat(*this);
		LUDecomposition(mat, index, d);
		for(int i = 0; i < _r; i++)
			det *= mat[i][i];

		return	det*d;
	}
}

template<typename T>
inline T CMatrix<T>::trace(void) const
{
	assert(this->isValid());
	assert(isSquare() && "ERROR - determinants are defined only for square matrices");
	
	// trace(A) = sum(a[i][i])
	T sum = 0;
	for(int i = 0; i < _r; i++)
		sum += _rowPtr[i][i];
	return sum;

}


/**
 * Returns a matrix that is the inverse of the current matrix.
 * uses a numerically stable Gauss¨CJordan elimination routine to compute the inverse.
 *
 *  The procedure is numerically unstable unless pivoting 
 * (exchanging rows and columns as appropriate) is used. 
 * Picking the largest available element as the pivot is usually a good choice. 
 *
 * @see
 * @return a new matrix that is the inverse of the current matrix
 * if the matrix is not invertible(it's singular) the matrix returned will
 * be invalid (has _r and _c set to negative);
 */
template<typename T>
inline CMatrix<T> CMatrix<T>::inverse (void) const 
{
	assert(this->isValid());
	assert(isSquare() && "ERROR - inverse are defined only for square matrices");

	// A = | a  b |
	//     | c  d |
	// |A| =  ad - bc 
	// inv(A) = 1/|A| * |  d  -b |
	//					| -c   a |
	if(_r == 2)
	{
		T di = T(1)/determinant();
		CMatrix<T>inv(2, 2);
		inv[0][0] = _rowPtr[1][1]*di;	 inv[0][1] = _rowPtr[0][1]*(-di);
		inv[1][0] = _rowPtr[1][0]*(-di); inv[1][1] = _rowPtr[0][0]*di;
		return inv;
	}
	// A = | a  b  c |
	//     | d  e  f |
	//     | g  h  i |
	// |A| = a(ei - fh) - b(di - fg) + c(dh - eg) 
	// inv(A) = 1/det * | ei-fh  ch-bi  bf-ce |
	//					| fg-di  ai-cg  cd-af |
	//                  | dh-eg  bg-ah  ae-bd |
	else if(_r == 3)
	{
		T di = T(1)/determinant();
		CMatrix<T>inv(3, 3);
		inv[0][0] = (_rowPtr[1][1]*_rowPtr[2][2] - _rowPtr[1][2]*_rowPtr[2][1])*di;
		inv[0][1] = (_rowPtr[0][2]*_rowPtr[2][1] - _rowPtr[0][1]*_rowPtr[2][2])*di;
		inv[0][2] = (_rowPtr[0][1]*_rowPtr[1][2] - _rowPtr[0][2]*_rowPtr[1][1])*di;
		inv[1][0] = (_rowPtr[1][2]*_rowPtr[2][0] - _rowPtr[1][0]*_rowPtr[2][2])*di;
		inv[1][1] = (_rowPtr[0][0]*_rowPtr[2][2] - _rowPtr[0][2]*_rowPtr[2][0])*di;
		inv[1][2] = (_rowPtr[0][2]*_rowPtr[1][0] - _rowPtr[0][0]*_rowPtr[1][2])*di;
		inv[2][0] = (_rowPtr[1][0]*_rowPtr[2][1] - _rowPtr[1][1]*_rowPtr[2][0])*di;
		inv[2][1] = (_rowPtr[0][1]*_rowPtr[2][0] - _rowPtr[0][0]*_rowPtr[2][1])*di;
		inv[2][2] = (_rowPtr[0][0]*_rowPtr[1][1] - _rowPtr[0][1]*_rowPtr[1][0])*di;
		return inv;
	}
	else
	{
		//// by GaussJordanElimination
		//CMatrix<T> dummyRightHandSide;
		//CMatrix<T> mat(*this);
		//GaussJordanElimination(mat, dummyRightHandSide);
		//return mat;

		// by LU decomposition
		CMatrix<T> mat(*this);
		CMatrix<T> inv = identity(_r);
		CVector<T> index(_r);
		int d;
		LUDecomposition(mat, index, d);
		// after calling LUDecomposition, index will be changed
		LUBacksubstitution(mat, inv, index);
		return inv;
	}
}

// ------------------------------------------------------
// Overloaded Operators 
// ------------------------------------------------------

/**
 * operator *
 * Multiply the mat to the right of the matrix.
 * @param mat reference to a CMatrix<T>
 * @return a matrix that is the product of the current matrix and mat.
 */
template<typename T>
CMatrix<T> CMatrix<T>::operator* (const CMatrix<T>& mat) const 
{
	assert(isMultiplicable(mat));	// _c == mat._r
	CMatrix<T> result(_r, mat._c);
	// result is initialized so that every entry in this matrix is 0

	for(int i = 0; i < _r; i++)
		for(int j = 0;j < mat._c; j++)
			for(int k=0; k < _c; k++)
				result[i][j] += _rowPtr[i][k] * mat[k][j];
	return result;
}

/**
 * operator *=
 * Multiplies the current matrix by mat on the right
 * @param mat reference to a matrix
 * @return reference to this 
 */
template<typename T>
CMatrix<T>& CMatrix<T>::operator*= (const CMatrix<T>& mat) 
{
	// in order for this operator to make sense
	// M and mat has to be both square and of same dimension
	assert(isSquare() && isCompatible(mat));

	CMatrix<T> result(_r, _c);
	for(int i = 0; i < _r; i++)
		for(int j = 0;j < _c; j++)
			for(int k=0; k < _c; k++)
				result[i][j] += _rowPtr[i][k]  * mat[k][j];
	*this = result;
	return (*this);
}


// ------------------------------------------------------
// Output or Test
// ------------------------------------------------------

/**
 * General test function for CMatrix<T> class
 * put anything you want to test here.
 */
template<typename T>
void CMatrix<T>::test()
{
	cout << "\n\nCMatrix Test\n=========================\n";

	CMatrix<T> m1(3, 3);
	//m1.reSize(3, 3);
	cout << m1 << endl;
	cout << "\nis symmetric?  " << m1.isSymmetric() << endl;

	m1[2][1] = 24;
	m1[0][2] = 15;
	m1[1][0] = -4;
	cout << m1 << endl;
	cout << "\nis symmetric?  " << m1.isSymmetric() << endl;
	
	CMatrix<T> m2 = m1.transpose();
	cout << m2 << endl;

	cout << "equal? " << (m1 == m2) << endl;
	cout << "not equal? " << (m1 != m2) << endl;

	CMatrix<T> m3 = identity(3);
	cout << m3 << endl; 

	CMatrix<T> m4 = m1*m3;
	cout << m4 << endl;

	m4 = m1*m1;
	cout << m4 << endl;

	CMatrix<T> m5(3, 5);
	cout << "\nm5 : " << m5 << endl;
	m5 = 2;
	CMatrix<T> m6 = m3*m5;
	cout << m6 << endl;
	m6[0][4] = 0.5;
	m6[2][1] = 16;
	m6[1][1] = 1;
	cout << m6 << endl;
	cout << m6.transpose() << endl;

	// single column matrix
	CMatrix<T> m7(1,4);
	m7[0][0] = 1;
	m7[0][1] = 2;
	m7[0][2] = 3;
	m7[0][3] = 4;
	cout << m7 << endl;
	cout << m7.transpose() << endl;

	CMatrix<T> m8(2, 2, 1);
	CMatrix<T> m9(2, 2, 2);
	CMatrix<T> m10(2, 2, 3);
	CMatrix<T> m11(m8);
	CMatrix<T> m12(m8);
	CMatrix<T> m13(2, 2);
	m8 += m9 * m10;

	cout << m8 << endl;

	m11 = m11 + m9*m10;
	cout << m11 << endl;

	m13 = m9*m10;
	cout << m13 << endl;

	m12 = m12+m13;
	cout << m12 << endl;

	CMatrix<T> m14(4, 4);
	CMatrix<T> m15(4, 2);

	m14[0][0] = 12; m14[0][1] = 32; m14[0][2] = 1.5f; m14[0][3] = 15;
	m14[1][0] = 2.5f;m14[1][1] = 17; m14[1][2] = 26;  m14[1][3] = 9;
	m14[2][0] = 8;  m14[2][1] = 23; m14[2][2] = 19;  m14[2][3] = 3;
	m14[3][0] = 12 ;m14[3][1] = 7.2f; m14[3][2] = 2;  m14[3][3] = 10;

	m15[0][0] = 20; m15[0][1] = 15;
	m15[1][0] = 6;  m15[1][1] = 6;
	m15[2][0] = 11; m15[2][1] = 19;
	m15[3][0] = 11; m15[3][1] = 22;

	CMatrix<T> m16 = m14 * m15;
	cout << "m16 : " << m16 << endl;

	CMatrix<T> m17 = m16 * m15.transpose();
	cout << "m17 : " << m17 << endl;
	cout << "m14 inv : " << m14.inverse() << endl;

	// matrix vector multiplication
	CVector<T> v3(3, 1);
	CVector<T> v4 = v3 * m5;
	cout << v4 << " = " << v3 << " * " << m5 << endl;

	CVector<T> v1(3, 2);
	CVector<T> v2 = m3 * v1;
	cout << v2 << " = " << m3 << " * " << v1 << endl;

	CMatrix<T> m18= matrix3x3(	0, 2, -3,
								2, 0, 1,
								1, -1, 3);
	cout << "\nm18 inv :\n" << m18.inverse();
	cout << "\nm18 det :\n" << m18.determinant();

	CMatrix<T> m19(3, 1); // right hand side
	m19[0][0] = 2;
	m19[1][0] = 3;
	m19[2][0] = 1;

	GaussJordanElimination(CMatrix<T>(m18), m19); 
	cout << "\nm19 rhs :\n" << m19;

	//CMatrix Test
	//=========================
	//m18 inv :
	//|   -0.25000     0.75000    -0.50000  |
	//|    1.25000    -0.75000     1.50000  |
	//|    0.50000    -0.50000     1.00000  |
	//m18 det : -4
	//m19 rhs :
	//|    1.25000  |
	//|    1.75000  |
	//|    0.50000  |
	// [l u p] = LU(m18)
	//l =
	//
	//    1.0000         0         0
	//         0    1.0000         0
	//    0.5000   -0.5000    1.0000
	//
	//
	//u =
	//
	//     2     0     1
	//     0     2    -3
	//     0     0     1
	//
	//
	//p =
	//
	//     0     1     0
	//     1     0     0
	//     0     0     1

	CMatrix<T> m20 = matrix2x2(	23, 12, 
								51, 21);
	cout << "\nm20 inv :\n" << m20.inverse();
	cout << "\nm20 det :\n" << m20.determinant();
	cout << "\nm20 trace : \n" << m20.trace();

	//m20 inv :
	//|   -0.16279     0.09302  |
	//|    0.39535    -0.17829  |
	//
	//m20 det :
	//-129.00000


	// Triangular matrix pack/unpack test
	// matrix norm test
	// matrix row/column vector extraction
	//=========================
	CMatrix<T> m21 = matrix3x3(10, 21, 33,
								21, 7, 12,
								33, 12, 6);
	CMatrix<T> m22 = matrix3x3 (10, 21, 33,
								12, 7, 10,
								33, 11, 6);

	cout << "\nm21 :\n" << m21;
	CVector<T> v21 = pack(m21);
	cout << "\nm21 packed form :\n" << v21;
	CMatrix<T> m23 = unpack(v21, 3);
	cout << "\nm21 unpack :\n" << m22;

	cout << "\nm22 norms\n--------------" << endl;
	cout << "\n1-norm : " << m22.norm1();
	cout << "\n2-norm : " << m22.norm2();
	cout << "\nInfinity-norm : "<< m22.normInfinity();
	cout << "\nFrobenius-norm : " << m22.normFrobenius();

	for(int i = 0; i < m22.rows(); i++)
		cout << endl << m22.rowVector(i);
	
	for(int j = 0; j < m22.cols(); j++)
		cout << endl << m22.colVector(j);

	// SVD and EVD test
	//=========================
	CMatrix<T> m24(5, 2);
	m24[0][0] = 20; m24[0][1] =  6;
	m24[1][0] = 15;	m24[1][1] =  6;
	m24[2][0] = 11; m24[2][1] = 11;
	m24[3][0] = 19; m24[3][1] = 22; 
	m24[4][0] =  6; m24[4][1] = 34;

	CMatrix<T> m25 = m24.transpose()*m24;
	cout <<  m25 << endl;
	cout << "\nSingular values :" << singularValues2(m24);
	cout << "\nEigen values :" << eigenValues2x2(m25);

	cout << "\nm25 norms" << endl;
	cout << "\n1-norm : " << m25.norm1();
	cout << "\n2-norm : " << m25.norm2();
	cout << "\nInfinity-norm : "<< m25.normInfinity();
	cout << "\nFrobenius-norm : " << m25.normFrobenius();

#if USE_MKL
	CMatrix<T> U;
	CMatrix<T> V;
	CVector<T> S;
	SVD(m22, U, S, V, true, true);
	cout << "\nSingular values by MKL :" << S;
	cout << "\nU : " << endl << U << endl;
	cout << "\nV : " << endl << V << endl;
#endif

	CVector<T> E;
	CMatrix<T> VD;
	EVD(m25, E, VD, false);
	cout << "\nEigen values : " << E;

#if USE_MKL
	EVD(m25, E, VD, true);
	cout << "\nEigen values by MKL :" << endl << E << endl;
	cout << "\nEigen vectors by MKL :" << endl << VD << endl;

	EVD(m21, E, VD, true);
	cout << "\nEigen values by MKL :" << endl << E << endl;
	cout << "\nEigen vectors by MKL :" << endl << VD << endl;
#endif
}

#endif // ___MATRIX__H___
