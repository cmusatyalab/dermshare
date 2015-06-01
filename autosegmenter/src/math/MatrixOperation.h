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

#ifndef ___MATRIX_OPERATION__H___
#define ___MATRIX_OPERATION__H___

#include <cassert>
#include <cstdio>
#include <cmath>
#include <cstring>
#include <iostream>
#include <iomanip>

#include "Vector.h"

// Most matrix factorization and decomposition are implemented
// efficiently in LAPACK. I decided to use Intel MKL (Math Kernel Library)
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

#ifndef SWAP
// susptable to round-off eror
#define SWAP(x, y) {(x)=(x)+(y); (y)=(x)-(y); (x)=(x)-(y);}
// requre an extra argument temp whose type may not be determined before hand
// #define SWAP(x, y) {T temp = (x); (x) = (y); (y) = temp;}
#endif
// "{ ... }" is used for SWAP being placed under for(;;) without parenthesis


#if USE_MKL

#ifndef MKL_6_0b
#define MKL_6_0b 0
#endif

#ifndef MKL_9_1
#define MKL_9_1 0
#endif


// intel math kernel library for LAPACK
#include "mkl_blas.h"
#include "mkl_lapack.h"

#if MKL_6_0b
extern "C"
{
	// MKL 6.0b
	 linear system solver
	void sspsv(char *uplo,int *n,int *nrhs,float *ap,int *ipiv,float *b,int *ldb,int *info);
	void dspsv(char *uplo,int *n,int *nrhs,double *ap,int *ipiv,double *b,int *ldb,int *info);

	// symmetric eigenvalue decomposition
	void ssyev(char *jobz,char *uplo,int *n,float *a,int *lda,float *w,float *work,int *lwork,int *info);
	void dsyev(char *jobz,char *uplo,int *n,double *a,int *lda,double *w,double *work,int *lwork,int *info);

	// singular value decomposition
	void sgesvd(char *jobu,char *jobvt,int *m,int *n,float *a,int *lda,float *s,float *u,int *ldu,float *vt,int *ldvt,float *work,int *lwork,int *info);
	void dgesvd(char *jobu,char *jobvt,int *m,int *n,double *a,int *lda,double *s,double *u,int *ldu,double *vt,int *ldvt,double *work,int *lwork,int *info);


	void sgemm(char *transa,char *transb,int *m,int *n,int *k,float *alpha,float *a,int *lda,float *b,int *ldb,float *beta,float *c,int *ldc);
	void dgemm(char *transa,char *transb,int *m,int *n,int *k,double *alpha,double *a,int *lda,double *b,int *ldb,double *beta,double *c,int *ldc);

	void sgees(char *jobvs,char *sort,void *select,int *n,float *a,int *lda,int *sdim,float *wr,float *wi,float *vs,int *ldvs,float *work,int *lwork,void *bwork,int *info);	
	void dgees(char *jobvs,char *sort,void *select,int *n,double *a,int *lda,int *sdim,double *wr,double *wi,double *vs,int *ldvs,double *work,int *lwork,void *bwork,int *info);
}
#endif //#if MKL_6_0

#if MKL_9_1
extern "C" 
{
	void sgemm(const char *transa, const char *transb, const MKL_INT *m, const MKL_INT *n, const MKL_INT *k,
			   const float *alpha, const float *a, const MKL_INT *lda, const float *b, const MKL_INT *ldb,
			   const float *beta, float *c, const MKL_INT *ldc);
	void dgemm(const char *transa, const char *transb, const MKL_INT *m, const MKL_INT *n, const MKL_INT *k,
			   const double *alpha, const double *a, const MKL_INT *lda, const double *b, const MKL_INT *ldb,
			   const double *beta, double *c, const MKL_INT *ldc);

	// linear system solver
	void sspsv(char *uplo,int *n,int *nrhs,float *ap,int *ipiv,float *b,int *ldb,int *info);
	void dspsv(char *uplo,int *n,int *nrhs,double *ap,int *ipiv,double *b,int *ldb,int *info);

	// symmetric eigenvalue decomposition
	void ssyev(char *jobz,char *uplo,int *n,float *a,int *lda,float *w,float *work,int *lwork,int *info);
	void dsyev(char *jobz,char *uplo,int *n,double *a,int *lda,double *w,double *work,int *lwork,int *info);

	// singular value decomposition
	void sgesvd(char *jobu,char *jobvt,int *m,int *n,float *a,int *lda,float *s,float *u,int *ldu,float *vt,int *ldvt,float *work,int *lwork,int *info);
	void dgesvd(char *jobu,char *jobvt,int *m,int *n,double *a,int *lda,double *s,double *u,int *ldu,double *vt,int *ldvt,double *work,int *lwork,int *info);

	void sgees(char *jobvs,char *sort,void *select,int *n,float *a,int *lda,int *sdim,float *wr,float *wi,float *vs,int *ldvs,float *work,int *lwork,void *bwork,int *info);	
	void dgees(char *jobvs,char *sort,void *select,int *n,double *a,int *lda,int *sdim,double *wr,double *wi,double *vs,int *ldvs,double *work,int *lwork,void *bwork,int *info);
}
#endif //#if MKL_9_1
#endif // USE_MKL


#include "Matrix.h" 

//// =======================================================================
//// utility functions
//// =======================================================================
//template<typename T> ostream&	operator<<				(ostream& os, const CMatrix<T>& mat);
//template<typename T> CMatrix<T> operator*				(T s, const CMatrix<T> & mat);
//template<typename T> CVector<T> operator*				(const CVector<T>& vec, const CMatrix<T>& mat);
//template<typename T> CVector<T> operator*				(const CMatrix<T>& mat, const CVector<T>& vec);
//template<typename T> bool		GaussJordanElimination	(CMatrix<T> &mat, CMatrix<T> &rightHandSide);
//template<typename T> bool		LUDecomposition			(CMatrix<T> &mat, CVector<T> &index, int &d);
//template<typename T> void		LUBacksubstitution		(const CMatrix<T> &LU, CMatrix<T> &RHS, const CVector<T> &index);
//
//template<typename T> CVector<T> pack					(const CMatrix<T> &mat);// for symmetric matrix, store in packed format
//template<typename T> CMatrix<T> unpack					(const CVector<T> &vec);
//template<typename T> CVector<T> eigenValues2x2			(const CMatrix<T> &mat);
//template<typename T> CVector<T> singularValues2			(const CMatrix<T> &mat);
//
//template<typename T> CVector<T> convertToVectorColumnMajor	(const CMatrix<T> &mat);
//template<typename T> void		convertToArrayColumnMajor	(const CMatrix<T> &mat, T* array);
//template<typename T> void		convertToArrayRowMajor		(const CMatrix<T> &mat, T* array);
//
//
//// ------------------------------------------------------
//// Eigenvector, Singular value decomposition, and other factorization
//// ------------------------------------------------------
//// Eigenvalue decomposition
//template<typename T> bool EVD(const CMatrix<T> &mat, CVector<T> &E, CMatrix<T> &V,	
//							  bool bStoreEigenVectors);
//
//// Singular Value Decomposition
//template<typename T> bool SVD(const CMatrix<T> &mat, CMatrix<T> &U, CVector<T> &S, CMatrix<T> &V, bool bStoreU, bool bStoreV);


// Eigen Value decomposition
// This routine takes in a symmetric matrix mat and 
// produces a diagonal real matrix E (vector format), with diagonal elements in decreasing order, 
// and a full matrix V whose columns are the corresponding eigenvectors so that mat*V = V*E.
// 
// @param mat symmetric matrix
// @param eigenvalues real diagonal matrix E stored in vector format.
// @param eigenvectors a full matrix V whose columns are the corresponding eigenvectors.
// @param bStoreEigenvectors flag indicating to store eigenvectors or not (default = false).
// @return bool successful or not
template<> 
bool EVD(const CMatrix<double> &mat, CVector<double> &E, CMatrix<double> &V,	
		 bool bStoreEigenVectors)
{
	assert(mat.isValid());
	assert(mat.isSquare() && "ERROR - eigenvalues are defined only for square matrices.");
	assert(mat.isSymmetric() && "ERROR - nonsymmetric eigenvalue decomposition has not been implemented.");

// if MKL library is installed
#if USE_MKL
	if(bStoreEigenVectors || mat.rows() != 2)
	{
		char jobz;
		char uplo = 'U'; // upper or lower triangular packed form
		int info;
		int n = mat.rows();
		int lda = n;
		int lwork = max(1, 3*n-1);
		
		double *w = new double[n];
		double *work = new double[lwork];

		// do we need eigen vectors
		if(bStoreEigenVectors) jobz = 'V';
		else jobz = 'N';
	
		//int len = int(n*(n+1)*0.5);
		double *a = new double[n*n];
		memcpy(a, mat.data(), n*n*sizeof(double));
		//memcpy(a, pack(mat).data(), len*sizeof(double));

		dsyev(&jobz, &uplo, &n, a, &lda, w, work, &lwork, &info);

		// notice that s memory is absorbed into CVector E and will be handled by the class destructor
		// no explicit deletion of w is needed
		E = CVector<double>(n, w);
		w = NULL;
		// since the eigenvalues returned is sorted in ascending order, we reverse it to get descending order
		E.reverse();

		if(bStoreEigenVectors)
		{
			V = CMatrix<double>(n, n, a);
			a = NULL;
			// transpose since mkl stores matrices in column major form.
			V = V.transpose();
		}

		delete[] work;
		work = NULL;

		if(info) return false;
		else return true;
	}
#endif // USE_MKL
	
	assert(mat.rows() == 2 && mat.cols() == 2 && "SORRY - eigenvalue of matrix more than 2x2 dimension hasn't been implemented");

	E = eigenValues2x2(mat);
	
	if(E.isValid()) return true;
	else return false;
}


template<> 
bool EVD(const CMatrix<float> &mat, CVector<float> &E, CMatrix<float> &V,	
		 bool bStoreEigenVectors)
{
	assert(mat.isValid());
	assert(mat.isSquare() && "ERROR - eigenvalues are defined only for square matrices.");
	assert(mat.isSymmetric() && "ERROR - nonsymmetric eigenvalue decomposition has not been implemented.");

// if MKL library is installed
#if USE_MKL
	if(bStoreEigenVectors || mat.rows() != 2)
	{
		char jobz;
		char uplo = 'U'; // upper or lower triangular packed form
		int info;
		int n = mat.rows();
		int lda = n;
		int lwork = max(1, 3*n-1);
		
		float *w = new float[n];
		float *work = new float[lwork];

		// do we need eigen vectors
		if(bStoreEigenVectors) jobz = 'V';
		else jobz = 'N';
	
		//int len = int(n*(n+1)*0.5);
		float *a = new float[n*n];
		memcpy(a, mat.data(), n*n*sizeof(float));
		//memcpy(a, pack(mat).data(), len*sizeof(float));

		ssyev(&jobz, &uplo, &n, a, &lda, w, work, &lwork, &info);

		// notice that s memory is absorbed into CVector E and will be handled by the class destructor
		// no explicit deletion of w is needed
		E = CVector<float>(n, w);
		w = NULL;
		// since the eigenvalues returned is sorted in ascending order, we reverse it to get descending order
		E.reverse();

		if(bStoreEigenVectors)
		{
			V = CMatrix<float>(n, n, a);
			a = NULL;
			// transpose since mkl stores matrices in column major form.
			V = V.transpose();
		}

		delete[] work;
		work = NULL;

		if(info) return false;
		else return true;
	}
#endif // USE_MKL
	
	assert(mat.rows() == 2 && mat.cols() == 2 && "SORRY - eigenvalue of matrix more than 2x2 dimension hasn't been implemented");

	E = eigenValues2x2(mat);
	
	if(E.isValid()) return true;
	else return false;
}


/// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
///=================================================================================
/// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
///
/// TODO:HOWARD 11-28-2006
/// Weird thing
/// 
/// If I leave the SVD routine in Matrix.h, it will cause an error (something like 
/// a heap corruption) in thinPlateSpline.cpp tps.solve() function. 
///
///
///
/// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
///=================================================================================
/// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

//Singular Value Decomposition
// This routine produces a diagonal matrix S (vector format), of the same dimension as mat and
// with nonnegative diagonal elements in decreasing order, and orthogonal matrix U and V so that
// mat = U * S * V'
// @param mat original matrix
// @param U left orthogonal vectors
// @param S singular values
// @param V right orthogonal vectors
// @param bStoreU 
// @param bStoreV
// @return bool successful or not
template<> 
bool SVD(const CMatrix<double> &mat, CMatrix<double> &U, CVector<double> &S, CMatrix<double> &V, bool bStoreU, bool bStoreV)
{
	assert(mat.isValid());

// if MKL library is installed
#if USE_MKL
	if(bStoreU || bStoreV || mat.cols() != 2)
	{
		char jobu;		
		char jobvt;
		int m = mat.rows();
		int n = mat.cols();
		int lmin = min(m, n);
		int lmax = max(m, n);

		int lda = m;
		int ldu = m;
		int ldvt = n;
		int lwork = max(3*lmin+lmax, 5*lmin);
		
		double *s = new double[lmin];
		double *u = NULL;
		double *vt = NULL;
		double *work = new double[lwork];
		int info;

		// do we need U
		if(bStoreU)	{jobu = 'A';u = new double[m*m];}
		else		jobu = 'N';
		
		// do we need V
		if(bStoreV)	{jobvt = 'A';vt = new double[n*n];}
		else		jobvt = 'N';

		CMatrix<double> A = mat.transpose();
		dgesvd(&jobu, &jobvt, &m, &n, A.data(), &lda, s, u, &ldu, vt, &ldvt, work, &lwork, &info);

		// notice that s memory is absorbed into CVector S and will be handled by the class destructor
		// no explicit deletion of s is needed
		S = CVector<double>(lmin, s); s = NULL;
		
		if(bStoreU)
		{
			U = CMatrix<double>(m, m, u); u = NULL;
			// transpose since mkl stores matrices in column major form.
			U = U.transpose();
		}
		
		if(bStoreV) V = CMatrix<double>(n, n, vt); vt = NULL;
		
		delete[] work; work = NULL;

		if(info) return false;
		else return true;
	}
#endif // USE_MKL
	
	assert(mat.cols() == 2 && "SORRY - SVD of a matrix not of mx2 dimension hasn't been implemented");

	S = singularValues2(mat);

	if(S.isValid()) return true;
	else return false;
}



template<> 
bool SVD(const CMatrix<float> &mat, CMatrix<float> &U, CVector<float> &S, CMatrix<float> &V, bool bStoreU, bool bStoreV)
{
	assert(mat.isValid());

// if MKL library is installed
#if USE_MKL
	if(bStoreU || bStoreV || mat.cols() != 2)
	{
		char jobu;		
		char jobvt;
		int m = mat.rows();
		int n = mat.cols();
		int lmin = min(m, n);
		int lmax = max(m, n);

		int lda = m;
		int ldu = m;
		int ldvt = n;
		int lwork = max(3*lmin+lmax, 5*lmin);
		
		float *s = new float[lmin];
		float *u = NULL;
		float *vt = NULL;
		float *work = new float[lwork];
		int info;

		// do we need U
		if(bStoreU){jobu = 'A';u = new float[m*m];}
		else		jobu = 'N';
		
		// do we need V
		if(bStoreV){jobvt = 'A';vt = new float[n*n];}
		else		jobvt = 'N';

		CMatrix<float> A = mat.transpose();
		sgesvd(&jobu, &jobvt, &m, &n, A.data(), &lda, s, u, &ldu, vt, &ldvt, work, &lwork, &info);

		// notice that s memory is absorbed into CVector S and will be handled by the class destructor
		// no explicit deletion of s is needed
		S = CVector<float>(lmin, s); s = NULL;
		
		if(bStoreU)
		{
			U = CMatrix<float>(m, m, u); u = NULL;
			// transpose since mkl stores matrices in column major form.
			U = U.transpose();
		}
		
		if(bStoreV)V = CMatrix<float>(n, n, vt); vt = NULL;

		delete[] work; work = NULL;

		if(info) return false;
		else return true;
	}
#endif // USE_MKL
	
	assert(mat.cols() == 2 && "SORRY - SVD of a matrix not of mx2 dimension hasn't been implemented");

	S = singularValues2(mat);
	
	if(S.isValid()) return true;
	else return false;
}




// =======================================================================
// utility functions
//
//
// =======================================================================
/**
* operator *
* Multiply the vec to the right of the matrix.
* @param vec reference to a CVector<T>
* @return a CVector<T> that is the product of the current matrix and vec.
*/
// vec * mat
template<typename T>
CVector<T> operator* (const CVector<T>& vec, const CMatrix<T>& mat)
{
	assert(vec.isValid() && mat.isValid() && vec.length() == mat.rows());
	CVector<T> result(mat.cols());
	for(int j = 0; j < mat.cols(); j++)
		for(int i=0; i< mat.rows(); i++)
			result[j] += vec[i] * mat[i][j];
	return result;
}

// mat * vec
template<typename T>
CVector<T> operator* (const CMatrix<T>& mat, const CVector<T>& vec)
{
	assert(vec.isValid() && mat.isValid() && vec.length() == mat.cols());
	CVector<T> result(mat.rows());
	for(int i = 0; i < mat.rows(); i++)
		for(int j=0; j < mat.cols(); j++)
			result[i] += mat[i][j] * vec[j];
	return result;
}


// RHS is replaced with the solution when returned
template<typename T>
bool GaussJordanElimination(CMatrix<T> &mat, CMatrix<T> &RHS)
{
	// matrix inverse is only defined for square matrix
	assert(mat.isValid());
	assert(mat.isSquare() && "ERROR - determinants are defined only for square matrices");

	// no right hand side, only calculate the inverse
	bool bRHS = RHS.isValid();
	if(bRHS)
		assert(RHS.rows() == mat.rows());

	//cout << "\nmat :\n" << mat;

	int irow = -1, icol = -1;
	int n = mat.rows();
	int m = RHS.cols();
	int* indxc = new int[n];
	int* indxr = new int[n];
	int* ipiv = new int[n];

	// set pivoting index to zero
	for (int j=0; j<n; ++j)
		ipiv[j]=0;

	// main loop over the columns to be reduced
	for (int i = 0; i < n; i++) 
	{
		T big = 0.0;
		// Outer loop of the search for a pivot element (picking the largest element in 
		// the matrix is a good practice)
		for (int j = 0; j < n; j++) 
		{
			if (ipiv[j] != 1) 
			{
				for (int k = 0; k < n; k++) 
				{
					if (ipiv[k] == 0) 
					{
						if (fabs(mat[j][k]) >= big) 
						{
							// Picking the largest available element as the pivot
							big = (T)(fabs(mat[j][k]));
							irow = j;
							icol = k;
						}
					}
					else if (ipiv[k] > 1)
					{
						cout << "\nError - Singular matrix in GaussJordanElimination";
						delete[] indxc;
						delete[] indxr;
						delete[] ipiv;
						return false;
					}
				}
			}
		}
		++ipiv[icol];
		// if the pivot chosen is not on the diagonal, swap rows to move it
		// to the diagonal
		// we now have the pivot element, so we interchange rows, if needed, to put the pivot element on the
		// diagonal. The columns are not physically interchanged, only relabeled: indx[i], the column of the
		// ith pivot element, is the ith column that is reduced, while indxr[i] is the row in which that
		// pivot element was originallly located. If indxr[i] != indxc[i[ there is an implied column interchange.
		// with this form of bookkeepoing, the solution b's will end up in the correct order, and the inverse
		// matrix will be scrambled by columns
		if (irow != icol) 
		{
			for (int k = 0; k < n; ++k)
				SWAP(mat[irow][k], mat[icol][k]);
			if(bRHS)
				for (int k = 0; k < m; ++k)
					SWAP(RHS[irow][k], RHS[icol][k]);
		}
		//cout << "\nmat swap " << irow << ", " << icol << " : \n" << mat;

		// record the row and column swapped at this stage
		indxr[i] = irow;
		indxc[i] = icol;
		
		if (mat[icol][icol] == 0.0)
		{
			cout << "\nError - Singular matrix in GaussJordanElimination";
			delete[] indxc;
			delete[] indxr;
			delete[] ipiv;
			return false;
		}
			
		// devide row icol by mat[icol][icol]
		T pivinv = T(1)/ mat[icol][icol];
		// TODO: here why do we devide the pivot by itself twice?
		// saving the I identity matrix here
		mat[icol][icol] = T(1); // replace the 1's with pivinv in the identity matrix and put it in A
		for (int j = 0; j < n; j++)
			mat[icol][j] *= pivinv;
		
		if(bRHS)
			for (int j = 0; j < m; j++)
				RHS[icol][j] *= pivinv;
		//cout << "\nmat devide by pivot " << 1/pivinv << " : \n" << mat;
		
		// Subtract this row from others to zero out their columns
		for (int j = 0; j < n; j++) 
		{
			if (j != icol) 
			{
				T save = mat[j][icol];
				mat[j][icol] = 0;	// replace the 0's with result in the identity matrix and put it in A
				for (int k = 0; k < n; k++)
					mat[j][k] -= mat[icol][k]*save;
				if(bRHS)
					for (int k = 0; k < m; k++)
						RHS[j][k] -= RHS[icol][k]*save;					
				//cout << "\nmat : reduce rows by row " << icol << " : \n" << mat;
			}
		}
	}
	// Swap columns to reflect permutation
	// This is the end of the main loop over columns of the reduction.
	// it only remains to unscramble the soulution in view of the clolumn interchanges.
	// we do this by interchanging pairs of columns int he reverse order that hte permutation
	// was builtup
	for (int j = n-1; j >= 0; j--) 
	{
		if (indxr[j] != indxc[j]) 
		{
			for(int k = 0; k < n; k++)
				SWAP(mat[k][indxr[j]], mat[k][indxc[j]]);
		}
	}

	delete[] indxc;
	delete[] indxr;
	delete[] ipiv;

	return true;
}

// mat is replaced with LU decomposition when returned
template<typename T>
bool LUDecomposition(CMatrix<T> &mat, CVector<T> &index, int &d)
{
	// matrix inverse is only defined for square matrix
	assert(mat.isValid());
	assert(mat.isSquare() && "ERROR - determinants are defined only for square matrices");
	assert(index.length() == mat.rows());

	int n = mat.rows(), imax;
	T big, dum, sum, temp;
	T *vv = new T[n]; // vv stores the implicit scaling of each row

	//cout << "\nmat :\n" << mat;
	
	d = 1; // no row interchanges yet
	// loop over rows to get the implicit scaling information
	for(int i = 0; i< n; i++)
	{
		big = 0;
		for(int j = 0; j < n; j++)
			if((temp = fabs(mat[i][j]))>big)
				big = temp;

		if(big == 0)
		{	// no nonzero largest element
			cout << "\nError - Singular matrix in LUDecomposition";
			delete[] vv;
			return false;
		}
		vv[i] = T(1)/big;	// save the scaling
	}
	// This is the loop over columns of Crout's method
	for(int j= 0; j < n; j++)
	{
		for(int i = 0; i < j; i++)
		{
			sum = mat[i][j];
			for(int k = 0; k < i; k++)
				sum -= mat[i][k]*mat[k][j];
			mat[i][j] = sum;
		}
		//cout << "\nmat i < j:\n" << mat;
		big = 0;
		// initialize for the search for largest pivot element
		for(int i = j; i < n; i++)
		{
			sum = mat[i][j];
			for(int k = 0; k < j; k++)
				sum -= mat[i][k]*mat[k][j];
			mat[i][j] = sum;

			// is the figure of merit for the pivot better than the best so far
			if((dum = vv[i]*fabs(sum)) >= big)
			{
				big = dum;
				imax = i;
			}
		}
		//cout << "\nmat i >= j:\n" << mat;

		// Do we need to interchange rows?
		if(j!=imax)
		{
			// yes, do so...
			for(int k = 0; k < n; k++)
			{
				dum = mat[imax][k];
				mat[imax][k] = mat[j][k];
				mat[j][k] = dum;
			}
			d = -d; // and change the parity of d
			vv[imax] = vv[j]; // also interchange the scale factor
		}
		//cout << "\nmat swap row " << imax << ", " << j << " :\n" << mat;

		index[j] = T(imax);
		if(mat[j][j] == 0.0)
			mat[j][j] = T(EPSILON);

		// if the pivot element is zero, the matrix is singular(at least to the precision of the algortihm).
		// for some applicaitons on singular matrices, it is desirable to substitute EPSILON for zero;
		if(j != n)
		{
			// divide by the pivot element
			dum = T(1)/mat[j][j];
			for(int i = j+1; i < n; i++)
				mat[i][j] *= dum;
		}
		//cout << "\nmat divide by pivot :\n"<< mat;
	} // go back for the next column in the reduction

	delete[] vv;
	return true;
}


template<typename T>
void LUBacksubstitution(const CMatrix<T> &LU, CMatrix<T> &RHS, const CVector<T> &index)
{
	assert(LU.isValid() && LU.isSquare());
	int n = LU.rows();

	assert(RHS.isValid() && RHS.rows() == n);
	assert(index.isValid() && index.length() == n);

	int ii, ip;
	T sum;
	
	// when ii is set to a non-negative value, it will become the index of the first
	// nonvanishing element of b. We now do the forward substitution. the only 
	// wrinkle is to unscramble the permutation as we go.
	for(int k = 0; k < RHS.cols(); k++)
	{
		ii = -1; // reset ii
		// column by column process the RHS
		for(int i = 0; i < n; i++)
		{
			ip = int(index[i]);
			sum = RHS[ip][k];
			RHS[ip][k] = RHS[i][k];
			if(ii != -1)
				for(int j =ii; j<= i-1; j++)
					sum -= LU[i][j]*RHS[j][k];
			else if(sum) // a nonzero element was encountered, so from now on we will have to do the sums in the loop above
				ii = i;
			RHS[i][k] = sum;
		}
		//cout << "\nRHS col " << k  << " :\n" << RHS;
		// now we do the backsubstitution
		for(int i = n-1; i >= 0; i--)
		{
			sum = RHS[i][k];
			for(int j = i+1; j < n; j++)
				sum -= LU[i][j] * RHS[j][k];
			RHS[i][k] = sum/LU[i][i];	// store a component of the solution vector x
		}
		//cout << "\nRHS col back" << k  << " :\n" << RHS;
	}
}



/**
 * for symmetric matrix, return a packed upper triangular form
 *
 * packed upper triangular matrix access : (M,i,j)   (M[(i+1)+(j+1)*((j+1)-1)/2 - 1])
 * size of packed form : n*(n+1)/2;  for n x n matrix
 **/
template<typename T>
inline CVector<T> pack(const CMatrix<T> &mat)
{
	assert(mat.isValid() && mat.isSymmetric());
	
	int n = mat.rows();
	int vs = n*(n+1)/2;
	CVector<T> vec(vs);
	
	for(int i=0; i < mat.rows(); i++)
		for(int j=i; j<mat.cols(); j++)
			vec[i+(j+1)*j/2] = mat[i][j];
	return vec;
}

/**
 * for symmetric matrix, return a packed upper triangular form
 *
 * packed upper triangular matrix access : (M,i,j)   (M[(i+1)+(j+1)*((j+1)-1)/2 - 1])
 * size of packed form : n*(n+1)/2;  for n x n matrix
 **/
template<typename T>
inline CMatrix<T> unpack(const CVector<T> &vec, int n)
{
	assert(vec.isValid());
	int vs = n*(n+1)/2;
	assert(vec.length() == vs && "Error - dimension mismatch in unpack");

	CMatrix<T> mat(n, n);
	for(int i=0; i < mat.rows(); i++)
		for(int j=i; j<mat.cols(); j++)
			mat[i][j] = mat[j][i] = vec[i+(j+1)*j/2];
	return mat;
}

template<typename T> 
inline CVector<T> convertToVectorColumnMajor(const CMatrix<T> &mat)
{
	assert(mat.isValid());
	int vs = mat.row()*mat.col();

	CVector<T> vec(vs);
	int count = 0;
	for(int j=0; j<mat.col(); j++)
		for(int i=0; i < mat.row(); i++)
			vec[count++] = mat[i][j];
	return vec;
}

//// assuming the appropriate memory is already alocated
//template<typename T> 
//inline void convertToArrayColumnMajor(const CMatrix<T> &mat, T* array)
//{
//	assert(mat.isValid());
//	int vs = mat.row()*mat.col();
//
//	int count = 0;
//	for(int j=0; j<mat.col(); j++)
//		for(int i=0; i < mat.row(); i++)
//			array[count++] = mat[i][j];
//}
//
//template<typename T> 
//inline void convertToArrayRowMajor(const CMatrix<T> &mat, T* array)
//{
//	assert(mat.isValid());
//	memcopy(array, mat.data(), mat.row()*mat.col()*sizeof(T));
//}



/**
 * calculate the eigenvalues of a 2x2 matrix
 * @return CVector<T>: the eigenvalues of the matrix are listed in a vector
 *  in the order of decending magnitude.
 * @todo handle complex eigenvalues.
 */
template<typename T>
CVector<T> eigenValues2x2(const CMatrix<T>&mat)
{
	// eigenvalues are defined only for square matrices
	assert(mat.isValid());
	assert(mat.isSquare() && "ERROR - eigenvalues are defined only for square matrices.");
	assert(mat.rows() == 2 && mat.cols() == 2 && "SORRY - eigenvalue of matrix more than 2x2 dimension hasn't been implemented");

	CVector<T> eig(2);
	T a = mat[0][0];
	T b = mat[0][1];
	T c = mat[1][0];
	T d = mat[1][1];

	// b^2 - 4ac
	T discriminant = b*c*T(4.) + (a-d)*(a-d);  

	// if not real, return an invalid matrix
	if(discriminant < 0)
	{
		cout << "SORRY - complex eigenvalues are not handled yet.\n";
		// since eig is not initialized, we can test whether it's invalid to check this case.
		return false;
	}

	// A = | a  b |
	//     | c  d |
	// eig1 = (a+d)/2 + sqrt(4*bc+(a-d)^2)/2
	// eig2 = (a+d)/2 - sqrt(4*bc+(a-d)^2)/2
	T ad = (a+d)*T(.5);
	T ds = sqrt(discriminant)*T(.5);
	if(ad >= 0) // eigenvalues are stored in decending orders in the eigenvalue vector
	{
		eig[0] = ad + ds;
		eig[1] = ad - ds;
	}
	else
	{
		eig[0] = ad - ds;
		eig[1] = ad + ds;
	}

	return eig;
}


/**
 * calculate the singular values of a mx2 or 2xm matrix.
 * The singular value of a mx2 matrix A is the sqrt of the eigenvalues of A'*A
 * sig{A} = sqrt(eig{A'*A})
 * The singular value of a mx2 matrix A is the sqrt of the eigenvalues of A*A'
 * sig(A) = sqrt(eig(A*A'))
 * @return CVector<T>: the singularvalues of a mx2 matrixare listed in a vector
 *  in the order of decending magnitude.
 * @todo handle complex eigenvalues.
 */
template<typename T>
CVector<T> singularValues2(const CMatrix<T>&mat)
{
	assert(mat.isValid());
	assert((mat.cols() == 2 || mat.rows() == 2)&& "SORRY - SVD of a matrix not of mx2 or 2xm dimension hasn't been implemented");

	CVector<T> sig;
	if(mat.cols() == 2)
		sig = eigenValues2x2(mat.transpose()*mat);
	else
		sig = eigenValues2x2(mat*mat.transpose());
	if( sig.isValid())
	{	sig[0] = sqrt(sig[0]);
		sig[1] = sqrt(sig[1]);
	}
	return sig;
}






#endif // ___MATRIX_OPERATION__H___
