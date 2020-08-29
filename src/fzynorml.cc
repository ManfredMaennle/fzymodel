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
 * $Id: fzynorml.cc,v 2.9 2020-08-29 10:47:38+00:00:02 manfred Exp $
 */


#ifndef WIN2017
#include <fstream.h>
#include <algo.h>
#else
#include <fstream>
// #include <numeric>
// #include <algorithm>
#endif

#include "global.hh"
#include "data.hh"
#include "main.hh"

#include "fzynorml.hh"

void 
fzynorml(char* infilename, char* outfilename)
  throw (Error) {
  
  const Real scale_interval_width = 1.25 * interval_width;

  Data a;
  a.load(infilename);

  size_t n_rows = a.U().size();
  if (n_rows < 1) {
    verbose(0, "warning: no data in file", infilename);
    return;
  }
  size_t n_columns = a.udim() + 1;
  if (n_columns < 1) {
    verbose(0, "warning: no data in file", infilename);
    return;
  }

  ofstream outfile;
  outfile.open(outfilename);
  if (!outfile) {
    throw FileOpenError(outfilename);
  }

  // /// scale factor/shift are computed based on ALL input patterns
  // /// (also on patterns not needed for training and validation data)
  int n_patterns_normalize = GLOBAL::scale_only_used_data;
  if (GLOBAL::scale_only_used_data == 0) {
    n_patterns_normalize = n_rows;
  }

  // /// calculate sum, max, min
  vector<Real> sum(n_columns, 0.0);
  vector<Real> min(n_columns, REAL_MAX);
  vector<Real> max(n_columns, -REAL_MAX);
  vector<Real>::iterator psum;
  vector<Real>::iterator pmin;
  vector<Real>::iterator pmax;
  vector<Uvector>::iterator paU = a.U().begin();  
  while (paU != a.U().begin() + n_patterns_normalize) {
    Uvector::iterator p = paU->begin();  
    psum = sum.begin();
    pmin = min.begin();
    pmax = max.begin();
    while (p != paU->end()) {
      if (*pmin > *p) *pmin = *p;
      if (*pmax < *p) *pmax = *p;
      ++pmin;
      ++pmax;
      *(psum++) += *(p++);
    }
    ++paU;
  }
  pmin = min.end() - 1;
  pmax = max.end() - 1;
  psum = sum.end() - 1;
  vector<Real>::iterator pay = a.y().begin();
  while (pay != a.y().begin() + n_patterns_normalize) {
    if (*pmin > *pay) *pmin = *pay;
    if (*pmax < *pay) *pmax = *pay;
    *psum += *pay;
    ++pay;
  }

  // /// calculate mean
  vector<Real> mean(n_columns, 0.0);
  vector<Real>::iterator pmean = mean.begin();
  psum = sum.begin();
  while (psum != sum.end()) {
    *(pmean++) = *(psum++) / n_patterns_normalize;
  }

  // /// compute standard deviation
  vector<Real> stddev(n_columns, 0.0);
  vector<Real>::iterator pstddev = stddev.begin();
  paU = a.U().begin();  
  while (paU != a.U().begin() + n_patterns_normalize) {
    pmean = mean.begin();
    pstddev = stddev.begin();
    Uvector::iterator p = paU->begin();  
    while (p != paU->end()) {
      Real diff = *(p++) - *(pmean++);
      *(pstddev++) += diff * diff; // variance
    }
    ++paU;
  }
  pstddev = stddev.end() - 1;
  pmean = mean.end() - 1;
  pay = a.y().begin();
  while (pay != a.y().begin() + n_patterns_normalize) {
    Real diff = *pay - *pmean;
    *pstddev += diff * diff;
    ++pay;
  }
  pstddev = stddev.begin();
  if (n_patterns_normalize > 1) {
    // Real n = n_patterns_normalize - 1.0;
    Real n = n_patterns_normalize;
    while (pstddev != stddev.end()) {
      *pstddev = sqrt(*pstddev / n);
      ++pstddev;
    }
  }
  else {
    while (pstddev != stddev.end()) {
      *pstddev = 1.0;
      ++pstddev;
    }
  }

  // /// calculate scale_shift ( = mean )
  vector<Real> scale_shift(n_columns, 0.0);
  vector<Real>::iterator pshift = scale_shift.begin();
  pmean = mean.begin();
  while (pshift != scale_shift.end()) {
    *(pshift++) = *(pmean++);
  }

  // /// calculate scale_factor
  vector<Real> scale_factor(n_columns, 0.0);
  vector<Real>::iterator pfactor = scale_factor.begin();
  if (GLOBAL::scale_by_standard_deviation) {
    // normalization factor
    pstddev = stddev.begin();
    while (pfactor != scale_factor.end()) {
      *pfactor = 0.5 * interval_width / (GLOBAL::sigma_factor * *pstddev);
      ++pfactor;
      ++pstddev;
    }
  }
  else if (GLOBAL::scale_by_range) {
    vector<Real>::const_iterator pmin = min.begin();
    vector<Real>::const_iterator pmax = max.begin();
    while (pfactor != scale_factor.end()) {
      Real range = *(pmax++) - *(pmin++);
      if (range != 0.0)  {
	*pfactor = scale_interval_width / range;
      }
      else {
	*pfactor = 1.0;
      }
      ++pfactor;
    }
  }
  else {
    throw Error("No scale method specified!"); 
  }

  outfile << "##source: \"" << infilename << "\"\n";
  outfile << "##rows: " << n_rows << "\n";
  outfile << "##columns: " << n_columns << "\n";
  outfile << "##shift: " << scale_shift << "\n";
  outfile << "##factor: " << scale_factor << "\n";
  outfile << "# min: " << min << "\n";
  outfile << "# max: " << max << "\n";
  outfile << "# mean: " << mean << "\n";
  outfile << "# stddev: " << stddev << "\n";
  if (GLOBAL::scale_by_standard_deviation) {
    outfile << "# scale by stddev; sigma_factor: " 
	    << GLOBAL::sigma_factor << "\n";
  }
  if (GLOBAL::scale_by_range) {
    outfile << "# scale by range; scale_interval_width: " 
	    << scale_interval_width << "\n";
  }
  outfile << scale_shift << endl;
  outfile << scale_factor << endl;

  outfile.close();
  return;
}
