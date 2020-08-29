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
 * $Id: page_hinkley.cc,v 2.9 2020-08-29 10:47:40+00:00:02 manfred Exp $
 *
 */

#include "page_hinkley.hh"

// ******************** Sprungdetektion

// gibt die erste Stelle n des Alarms zurueck
// Funktionswert ist die Anzahl der Alarme
// Alarm, wenn (U_n - m_n) >= lambda
// Quelle: Basseville 86

int 
page_hinkley_increase(Matrix<Real>& x, Real mu_0, Real nu, Real lambda,int& n){
  const int col = 0;
  Matrix<Real> U(x.num_rows(), x.num_cols());
  Real min_n = 0.0;
  int k;
  int alarme = 0;
  n = 0;
  U[0][col] = x[0][col] - mu_0 - 0.5*nu;
  if (U[0][col] < min_n) {
    min_n = U[0][col];
  }
  for(k=1; k<x.num_rows(); k++) {
    U[k][col] = U[k-1][col] + x[k][col] - mu_0 - 0.5*nu;
    //PRINT(U[k][col]);
 cout << U[k][col] << endl;
    if (U[k][col] < min_n) {
	min_n = U[k][col];
    }
    if ( (U[k][col] - min_n) >= lambda ) { 
      if (n == 0) {
	n = k+1;
      }
      alarme++;
    } 
  }
  return alarme;
}

int 
page_hinkley_decrease(Matrix<Real>& x, Real mu_0, Real nu, Real lambda,int& n){
  const int col = 0;
  Matrix<Real> T(x.num_rows(), x.num_cols());
  Real max_n = 0.0;
  int k;
  int alarme = 0;
  n = 0;
  T[0][col] = x[0][col] - mu_0 + 0.5*nu;
  if (T[0][col] > max_n) {
    max_n = T[0][col];
  }
  for(k=1; k<x.num_rows(); k++) {
    T[k][col] = T[k-1][col] + x[k][col] - mu_0 + 0.5*nu;
    //PRINT(T[k][col]);
 cout << T[k][col] << endl;
    if (T[k][col] > max_n) {
	max_n = T[k][col];
    }
    if ( (max_n - T[k][col]) >= lambda ) { 
      if (n == 0) {
	n = k+1;
      }
      alarme++;
    } 
  }
  return alarme;
}

// Sprungdetektor; gleitender Mittelwert, Fensterbreite = d
int 
threshold_jump_increase(Matrix<Real>& x, Real mu_0, Real nu, int d, int& n){
  const int col = 0;
  int k;
  int alarme = 0;
  Real sum = 0.0;
  Real mu_1 = (mu_0 + nu) * d;
  n = 0;
  for(k=0; k<d; k++) {
    sum += x[k][col];
  }
  for(k=0; k<x.num_rows()-d; k++) { 
    sum -= x[k][col];
    sum += x[k+d][col];
    if (sum > mu_1) {
      if (n == 0) {
	n = d + k;
      }
      alarme++;
    }
  }
  return alarme;
}

// Sprungdetektor; gleitender Mittelwert, Fensterbreite = d
int 
threshold_jump_decrease(Matrix<Real>& x, Real mu_0, Real nu, int d, int& n){
  const int col = 0;
  int k;
  int alarme = 0;
  Real sum = 0.0;
  Real mu_1 = (mu_0 - nu) * d;
  n = 0;
  for(k=0; k<d; k++) {
    sum += x[k][col];
  }
  for(k=0; k<x.num_rows()-d; k++) { 
    sum -= x[k][col];
    sum += x[k+d][col];
    if (sum < mu_1) {
      if (n == 0) {
	n = d + k;
      }
      alarme++;
    }
  }
  return alarme;
}

