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
 * $Id: svd.cc,v 2.9 2020-08-29 10:47:40+00:00:02 manfred Exp $
 */

/*
 * Last changes by Manfred Maennle.
 *
 * Original version: svd.h v1.5 from Steffen Bloedt.
 */

/*
 * This function computes the Singular Value Decomposition (SVD) 
 * of a given matrix. The SVD algorithm is used to solve the
 * least squares problem of overdetermined linear equation systems.
 *
 * The algorithm is from:
 * J.H. Wilkinson, C.Reinsch: Linear Algebra, Springer 1971, p. 134ff
 *
 * svd.h
 *
 * @author: Steffen Bloedt
 * @version: Revision: 1.5  Date: 1999/05/12 09:31:35 
 */



#ifndef WIN2017
#include <vec.h>     // TNT vectors
#else
#include <tnt.h>     // TNT vectors
#endif



#include "svd.hh"


/** Compute the Singular Value Decomposition (SVD) of a m x n matrix A.
 *
 * @param A  The input matrix.
 * @param Q  The diagonal matrix.
 * @param U  A matrix with orthonormalized columns.
 * @param V  An orthogonal matrix.
 *
 * @exception SVDException  This exception will be thrown when
 *                          an error occurs during the computation.
 *
 */

void 
SVD(const Matrix<Real>& A, Matrix<Real>& Q, Matrix<Real>& U, Matrix<Real>& V)
  throw (SVDError)
{
  /** 
   * Computation of the singular values Q and complete orthogonal
   * decomposition (U, V) of a real rectangular matrix A with
   * A = U * Q * V.transp(); U.transp() * U = V.transp() * V = I.
   * 
   * Solve a least squares problem given by the m equations
   *    y = A * p
   * with
   *    p = V * Q.inverse() * U.transp() * y
   * where Q.inverse() is easy to compute since Q is a diagonal
   * matrix.
   *
   * Parameters:
   *  A: m x n input matrix to decompose; m rows, n columns
   *  U: m x n matrix with orthonormalized columns
   *  Q: n x n diagonal matrix holding the singular values in its diagonal
   *  V: n x n matrix with orthogonal columns
   *
   * Precondition: m >= n.
   */
  int m = A.num_rows();
  int n = A.num_cols();
  if (m < n) {
    throw SVDError();
  }
  
  /**
   *  Define REAL_EPSILON, REAL_MIN to FLT_EPSILON, FLT_MIN
   *  or DBL_EPSILON, DBL_MIN respectively.
   */
  Real eps = REAL_EPSILON;
  Real tol = REAL_MIN / eps;

  int i, j, k, l1;
  int l = 0;
  Real c, f, g, h, s, x, y, z;

  Vector<Real> e(n);
  Vector<Real> q(n); // This one will contain the diagonal elements.
  
  U = A; // This copies the matrix values, not only an object reference!
  
  // Resize Q and V if necessary:
  if (Q.num_rows() != n || Q.num_cols() != n) {
    Q.newsize(n, n);
  }
  if (V.num_rows() != n || V.num_cols() != n) {
    V.newsize(n, n);
  }
  
  for (i = 1; i <= n; i++) {
    for (j = 1; j <= n; j++) {
      Q(i, j) = 0;
      V(i, j) = 0;
    }
  }
  
  // Householder's reduction to bidiagonal form:
  g = 0;
  x = 0;

  for (i = 1; i <= n; i++) {
    e(i) = g;
    s = 0;
    l = i + 1;

    for (j = i; j <= m; j++) {
      s = s + (U(j, i) * U(j ,i));
    }

    if (s < tol) {
      g = 0;
    }
    else {
      f = U(i, i);      
      g = (f < 0) ? sqrt(s) : -sqrt(s);
      h = f * g - s;
      U(i, i) = f - g;

      for (j = l; j <= n; j++) {
	s = 0;

	for (k = i; k <= m; k++) {
	  s = s + (U(k, i) * U(k, j));
	}
	f = s / h;
	for (k = i; k <= m; k++) {
	  U(k, j) = U(k, j) + (f * U(k, i));
	}
	
      } // j
    } // end else
    q(i) = g;
    s = 0;

    for (j = l; j <= n; j++) {
      s = s + (U(i, j) * U(i, j));
    }

    if (s < tol) {
      g = 0;
    }
    else {
      f = U(i, i + 1);
      g = (f < 0) ? sqrt(s) : -sqrt(s);
      h = (f * g) - s;
      U(i, i + 1) = f - g;

      for (j = l; j <= n; j++) {
	e(j) = U(i, j) / h;
      }
      for (j = l; j <= m; j++) {
	s = 0;
	for (k = l; k <= n; k++) {
	  s = s + (U(j, k) * U(i, k));
	}
	for (k = l; k <= n; k++) {
	  U(j, k) = U(j, k) + (s * e(k));
	}
      } // j 
    } // end else
    y = fabs(q(i)) + fabs(e(i));
    if (y > x) {
      x = y;
    }
  } // i

  // Accumulation of right-hand transformations:
  for (i = n; i >= 1; i--) {
    if (g != 0) {
      h = U(i, i+1) * g;
      for (j = l; j <= n; j++) {
	V(j, i) = U(i, j) / h;
      }
      for (j = l; j <= n; j++) {
	s = 0;
	for (k = l; k <= n; k++) {
	  s = s + U(i, k) * V(k, j);
	}
	for (k = l; k <= n; k++) {
	  V(k, j) = V(k, j) + (s * V(k, i));
	}
      } // j
    } // end if

    for (j = l; j <= n; j++) {
      V(i, j) = 0;
      V(j, i) = 0;
    }
    V(i, i) = 1;
    g = e(i);
    l = i;
  } // i  

  // Accumulation of left-hand transformations:
  for (i = n; i >= 1; i--) {
    l = i + 1;
    g = q(i);
    for (j = l; j <= n; j++) {
      U(i, j) = 0;
    }
    if (g != 0) {
      h = U(i, i) * g;
      for (j = l; j <= n; j++) {
	s = 0;
	for (k = l; k <= m; k++) {
	  s = s + (U(k, i) * U(k, j));
	}
	f = s / h;
	for (k = i; k <= m; k++) {
	  U(k, j) = U(k, j) + (f * U(k, i));
	}
      } // j
      for (j = i; j <= m; j++) {
	U(j, i) = U(j, i) / g;
      }
    } // end if
    else {
      for (j = i; j <= m; j++) {
	U(j, i) = 0;
      }
    }
    U(i, i) = U(i, i) + 1;
  } // i
  
  // Diagonalization of the bidiagonal form:
  eps = eps * x;
  for (k = n; k >= 1; k--) {
  test_f_splitting:                                 // <-- test_f_splitting
    for (l = k; l >= 1; l--) {
      if (fabs(e(l)) <= eps)
	{
	  goto test_f_convergence;
	}
      if (fabs(q(l - 1)) <= eps) 
	{
	  goto cancellation;
	}
    } // l

    // Cancellation of e(l) if l > 1:
  cancellation:                                     // <-- cancellation
    c = 0;
    s = 1;
    l1 = l - 1;
    for (i = l; i <= k; i++) {
      f = s * e(i);
      e(i) = c * e(i);
      if (fabs(f) <= eps) {
	goto test_f_convergence;
      }
      g = q(i);
      h = sqrt((f * f) + (g * g));
      q(i) = h;
      c = g / h;
      s = -f / h;
      for (j = 1; j <= m; j++) {
	y = U(j, l1);
	z = U(j, i);
	U(j, l1) = (y * c) + (z * s);
	U(j, i) = (-y * s) + (z * c);
      } // j
    } // i
  test_f_convergence:                               // <-- test_f_convergence
    z = q(k);
    if (l == k) {
      goto convergence;
    }

    // Shift from bottom 2x2 minor:
    x = q(l);
    y = q(k - 1);
    g = e(k - 1);
    h = e(k);
    f = ((y - z) * (y + z) + (g - h) * (g + h)) / (2 * h * y);
    g = sqrt((f * f) + 1);
    f = ((x - z) * (x + z) + h * (y / ((f < 0)?(f - g):(f + g)) - h)) / x;

    // Next QR transformation:    
    c = 1;
    s = 1;
    for (i = l + 1; i <= k; i++) {
      g = e(i);
      y = q(i);
      h = s * g;
      g = c * g;
      z = sqrt((f * f) + (h * h));
      e(i - 1) = z;
      c = f / z;
      s = h / z;
      f = (x * c) + (g * s); 
      g = (-x * s) + (g * c);
      h = y * s;
      y = y * c;
      for (j = 1; j <= n; j++) {
	x = V(j, i - 1);
	z = V(j, i);
	V(j, i - 1) = (x * c) + (z * s);
	V(j, i) = (-x * s) + (z * c);
      } // j
      z = sqrt((f * f) + (h * h));
      q(i - 1) = z;
      c = f / z;
      s = h / z;
      f = (c * g) + (s * y);
      x = (-s * g) + (c * y);
      for (j = 1; j <= m; j++) {
	y = U(j, i - 1);
	z = U(j, i);
	U(j, i - 1) = (y * c) + (z * s);
	U(j, i) = (-y * s) + (z * c);
      } // j
    } // i
    e(l) = 0;
    e(k) = f;
    q(k) = x;
    goto test_f_splitting;
  convergence:                                      // <-- convergence
    if (z < 0) {
      // q(k) is made non-negative:
      q(k) = -z;
      for (j = 1; j <= n; j++){
	V(j, k) = -V(j, k);
      }
    } // end if
  } // k
  
  // Transfer the diagonal values from q to Q:
  for (k = 1; k <= n; k++) {
    Q(k, k) = q(k);
  }
  
  return;
}
