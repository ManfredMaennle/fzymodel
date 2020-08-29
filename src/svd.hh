#ifndef SVD_HH
#define SVD_HH

/*
 * MIT License
 *
 * Copyright (c) 1999, 2020 Manfred Maennle
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * $Id: svd.hh,v 2.9 2020-08-29 10:47:40+00:00:02 manfred Exp $
 */


/*
 * Last changes by Manfred Maennle.
 *
 * Original version:  svd.h (Revision: 1.5  Date: 1999/05/12 09:31:35)
 * from Steffen Bloedt.
 *
 */


#ifndef WIN2017
#include <string>
#include <cmat.h>
#else
#include <string>
#include <math.h>
#endif


// Include definition of "Real" and corresponding constants (REAL_MIN, etc.).
// Include definition of Error abstract class.
// Include definition of SVDError class.
#include "global.hh"  

// set constants, if not yet done
// #ifndef REAL_MIN
// // use double
// typedef double Real;
// #define REAL_MAX DBL_MAX
// #define REAL_MIN DBL_MIN
// #define REAL_EPSILON DBL_EPSILON
// #endif


//#define TNT_BOUNDS_CHECK // uncomment this line for debugging


using namespace std;
using namespace TNT;


/** Compute the Singular Value Decomposition (SVD) of a m x n matrix A.
 *
 * This function computes the Singular Value Decomposition (SVD) 
 * of a given matrix A. Precondition: m >= n. 
 * The SVD algorithm can be used to solve the
 * least squares problem of overdetermined linear equation systems.
 *
 * The algorithm is taken from:
 * J.H. Wilkinson, C.Reinsch: Linear Algebra, Springer 1971, p. 134ff
 *
 * The function computes the singular values Q and the complete orthogonal
 * decomposition (U, V) of a real rectangular matrix A with
 * A = U * Q * V.transp(); U.transp() * U = V.transp() * V = I.
 * 
 * With this, you can solve a linear least squares problem 
 * given by the m equations
 *    y = A * p
 * with
 *    p = V * Q.inverse() * U.transp() * y
 * where Q.inverse() is easy to compute, since Q is a diagonal
 * matrix.
 *
 * @param A m x n input matrix to decompose; m rows, n columns. 
 *    Precondition: m >= n. 
 * @param Q n x n diagonal matrix holding the singular values in its diagonal
 *    (return value).
 * @param U m x n matrix with orthonormalized columns (return value).
 * @param V n x n matrix with orthogonal columns (return value).
 *
 * @exception SVDException  This exception will be thrown when
 *                          an error occurs during the computation.
 * @author Steffen Bloedt, (minor changes by Manfred M\"annle)
 * @memo
 */

void 
SVD(const Matrix<Real>& A, Matrix<Real>& Q, Matrix<Real>& U, Matrix<Real>& V)
  throw (SVDError);

#endif /* #ifndef SVD_HH */
