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
 * $Id: fzymkdat.cc,v 2.9 2020-08-29 10:47:38+00:00:02 manfred Exp $
 */


#ifndef WIN2017
#include <fstream.h>
#include <algo.h>
#else
#include <fstream>
//#include <numeric>
#include <algorithm>
#endif

#include "global.hh"
#include "data.hh"
#include "main.hh"

#include "fzymkdat.hh"

void 
fzymkdat(char* normfilename, char* infilename, char* outfilename)
  throw (Error) {
  // /// flag: generate dynamic (or static) data
  int dynamic = 1;
  if (GLOBAL::dynamic_orders.size() < 1) {
    return;
  }
  if (*(GLOBAL::dynamic_orders.begin()) == 0) {
    // /// build static data
    dynamic = 0;
  }

  cerr << "********* dynamic: " << dynamic << endl;
  
  // /// load data
  Data normalization;
  normalization.load(normfilename);
  Data a;
  a.load(infilename);
  size_t n_rows = a.U().size();
  if (n_rows < 1) {
    return;
  }
  size_t n_columns = a.udim() + 1;
  if (n_columns < 1) {
    return;
  }

  // /// check orders
  if (dynamic && (GLOBAL::dynamic_orders.size() != 1+a.udim())) {
    throw Error("no. of dynamic orders != columns in input data file!");
  }
  int min_order =  *min_element(GLOBAL::dynamic_orders.begin(), 
				GLOBAL::dynamic_orders.end());
//   if (dynamic && (min_order < 1)) {
//     throw Error("dynamic order < 1 at -d option!");
//   }
  if (dynamic && (min_order < 0)) {
    throw Error("dynamic order < 0 at -d option!");
  }
  size_t n_columns_dynamic = accumulate(GLOBAL::dynamic_orders.begin(), 
					GLOBAL::dynamic_orders.end(), 1);
  if (dynamic && (n_columns_dynamic < 2)) {
    throw Error("no input variable columns < 1 at -d option!");
  }
  size_t max_order =  *max_element(GLOBAL::dynamic_orders.begin(), 
				   GLOBAL::dynamic_orders.end());


  size_t n_skip_data = GLOBAL::n_skip_data;
  size_t n_patterns_net = GLOBAL::n_training_data;
  if (GLOBAL::n_training_data == 0) {
    n_patterns_net = n_rows;
    if (n_patterns_net > n_skip_data) {
      n_patterns_net -= n_skip_data;
    }
    else {
      throw Error("not enough patterns in infile or n_skip_data too high!");
    }
    if (dynamic) {
      if (n_patterns_net > max_order) {
	n_patterns_net -= max_order;
      }
      else {
	throw Error("not enough patterns in infile or max_order too high!");
      }
    }
  }
  
  size_t n_patterns_gross = n_patterns_net + max_order;
  if (n_patterns_gross + GLOBAL::n_skip_data > n_rows) {
    throw Error("not enough patterns in input data file!");
  }
  if (n_patterns_gross + GLOBAL::n_skip_data < n_rows) {
    string msg = itos(n_rows - n_patterns_gross - GLOBAL::n_skip_data);
    msg += " unused patterns in input data file!";
    verbose(0, "warning", msg);
  }
  string column_names;
  string dynamics_names;

  ofstream trainingfile_norm;
  ofstream trainingfile_orig;
  string trainingfilename_norm = (string)outfilename + ".nrm";
  trainingfile_norm.open(trainingfilename_norm.c_str());
  if (!trainingfile_norm) {
    throw FileOpenError(trainingfilename_norm);
  }
  if (! GLOBAL::denormalize) {
    string trainingfilename_orig = (string)outfilename + ".ogl";
    trainingfile_orig.open(trainingfilename_orig.c_str());
    if (!trainingfile_orig) {
      throw FileOpenError(trainingfilename_orig);
    }
  }

  // /// normalize a
  a.normalize(normalization.scale_factor(),
	      normalization.scale_shift());

  // /// write static data
  if (! dynamic) {
    for (size_t col = 0; col < a.udim(); ++ col) {
      column_names += (string)" u" + itos(col+1);
    }
    column_names += (string)" y";
    trainingfile_norm << "##source: \"" << infilename << "\"\n";
    trainingfile_norm << "##rows: " << n_patterns_net << "\n";
    trainingfile_norm << "##columns: " << n_columns << "\n";
    trainingfile_norm << "##dynamics: 0\n";
    trainingfile_norm << "##factor: " << a.scale_factor() << "\n";
    trainingfile_norm << "##shift: " << a.scale_shift() << "\n";
    trainingfile_norm << "##names:" << column_names << "\n";
    if (! GLOBAL::denormalize) {
      vector<Real> zeros(n_columns, 0.0);
      vector<Real> ones(n_columns, 1.0);
      trainingfile_orig << "##source: \"" << infilename << "\"\n";
      trainingfile_orig << "##rows: " << n_patterns_net << "\n";
      trainingfile_orig << "##columns: " << n_columns << "\n";
      trainingfile_orig << "##dynamics: 0\n";
      trainingfile_orig << "##factor: " << ones << "\n";
      trainingfile_orig << "##shift: " << zeros << "\n";
      trainingfile_orig << "##names:" << column_names << "\n";
    }
    vector<Uvector>::const_iterator paU = a.U().begin();  
    vector<Real>::const_iterator pay = a.y().begin();
    for (size_t row = 0; row < n_skip_data; ++row) {
      ++paU;
      ++pay;
    }
    for (size_t row = 0; row < n_patterns_net; ++row) {
      trainingfile_norm << *paU << *pay << endl;
      if ((a.blocksize() > 0) && ((row+1) % a.blocksize() == 0)) {
	trainingfile_norm << endl;
      }
      if (! GLOBAL::denormalize) {
	//#WIN2017 vector<Real>::const_iterator pau = paU->begin();
	Uvector::const_iterator pau = paU->begin();
	vector<Real>::const_iterator pf = a.scale_factor().begin();
	vector<Real>::const_iterator ps = a.scale_shift().begin();
	while (pau != paU->end()) {
	  trainingfile_orig << denormalization(*(pau++), *(pf++), *(ps++))
			    <<" ";
	}
	trainingfile_orig << denormalization(*pay, *pf, *ps) << endl;
	if ((a.blocksize() > 0) && ((row+1) % a.blocksize() == 0)) {
	  trainingfile_orig << endl;
	}
      }
      ++paU;
      ++pay;
    }
    trainingfile_norm.close();
    if (! GLOBAL::denormalize) {
      trainingfile_orig.close();
    }
    return;
  }

  // /// write dynamic data

  // /// create matrix b containing shifted columns (dynamics) 
  Data b(n_columns_dynamic-1, n_patterns_gross-1, 0.0);
  vector<Uvector>::iterator pbU = b.U().begin();  
  vector<Real>::iterator pby = b.y().begin();
  vector<Uvector>::const_iterator paU = a.U().begin();  
  vector<Real>::const_iterator pay = a.y().begin();
  for (size_t row = 0; row < n_skip_data; ++row) {
    ++paU;
    ++pay;
  }
  if (GLOBAL::ut_regressor) {
    ++paU;
  }
  vector<Real> last_row(n_columns_dynamic-1, 0.0); // last row of U
  for (size_t row = 0; row < n_patterns_gross - 1; ++row) {
    vector<int>::reverse_iterator prorder = GLOBAL::dynamic_orders.rbegin();
    vector<Real>::reverse_iterator prlast_row = last_row.rbegin();
    if (*prorder > 0) {
      // save y-2, y-3, ...
      for (int col = 1; col < *prorder; ++col) {
	*prlast_row = *(++prlast_row);
      }
      // save y-1
      *(prlast_row++) = *pay;
    }
    ++prorder;
    // unfortunately, no reverse iterators for Uvector
    Uvector::iterator prau = paU->end() - 1;
    while (prorder != GLOBAL::dynamic_orders.rend()) {
      if (*prorder > 0) {
	// save x-2, x-3, ...
	for (int col = 1; col < *prorder; ++col) {
	  *prlast_row = *(++prlast_row);
	}
	// save x-1
	*(prlast_row++) = *prau;
      }
      --prau;
      ++prorder;
    }
    // save last_row to b
    Uvector::iterator pbu = pbU->begin();
    vector<Real>::iterator plast_row = last_row.begin();
    while (plast_row != last_row.end()) {
      *(pbu++) = *(plast_row++);
    }
    // save y
    *pby = *(pay + 1);
    ++pbU;
    ++paU;
    ++pay;
    ++pby;
  }

  // /// create factor and shift vector (containing columns due to dynamics)
  vector<Real> scale_factor_dynamic;
  vector<Real> scale_shift_dynamic;
  scale_factor_dynamic.resize(n_columns_dynamic);
  scale_shift_dynamic.resize(n_columns_dynamic);
  vector<Real>::iterator pf = scale_factor_dynamic.begin();
  vector<Real>::iterator ps = scale_shift_dynamic.begin();
  vector<Real>::const_iterator pfactor = a.scale_factor().begin();
  vector<Real>::const_iterator pshift = a.scale_shift().begin();
  vector<int>::const_iterator porder = GLOBAL::dynamic_orders.begin();
  while (porder != GLOBAL::dynamic_orders.end()) {
    for (int k = 0; k < *porder; ++k) {
      *(pf++) = *pfactor;
      *(ps++) = *pshift;
    }
    ++pfactor;
    ++pshift;
    ++porder;
  }
  // save scale of y
  *pf = *(a.scale_factor().end() - 1);
  *ps = *(a.scale_shift().end() - 1);

  // /// write output files
  for (size_t col = 0; col < a.udim(); ++ col) {
    for (int order = 0; order < GLOBAL::dynamic_orders[col]; ++order) {
      column_names += (string)" u" + itos(col+1) + "(t-";
      column_names += itos(order+1) + ")";
    }
  }
  for (int order = 0; order < *(GLOBAL::dynamic_orders.end()-1); ++order) {
    column_names += (string)" y(t-" + itos(order+1) + ")";
  }
  column_names += (string)" y(t)";
  for (vector<int>::iterator porder = GLOBAL::dynamic_orders.begin();
       porder != GLOBAL::dynamic_orders.end();
       ++porder) {
    dynamics_names += (string)" " + itos(*porder);
  }
  trainingfile_norm << "##source: \"" << infilename << "\"\n";
  trainingfile_norm << "##rows: " << n_patterns_net << "\n";
  trainingfile_norm << "##columns: " << n_columns_dynamic << "\n";
  trainingfile_norm << "##dynamics:" << dynamics_names << "\n";
  trainingfile_norm << "##factor: " << scale_factor_dynamic << "\n";
  trainingfile_norm << "##shift: " << scale_shift_dynamic << "\n";
  trainingfile_norm << "##names:" << column_names << "\n";
  if (! GLOBAL::denormalize) {
    vector<Real> zeros(n_columns_dynamic, 0.0);
    vector<Real> ones(n_columns_dynamic, 1.0);
    trainingfile_orig << "##source: \"" << infilename << "\"\n";
    trainingfile_orig << "##rows: " << n_patterns_net << "\n";
    trainingfile_orig << "##columns: " << n_columns_dynamic << "\n";
    trainingfile_orig << "##dynamics: " << dynamics_names << "\n";
    trainingfile_orig << "##factor: " << ones << "\n";
    trainingfile_orig << "##shift: " << zeros << "\n";
    trainingfile_orig << "##names:" << column_names << "\n";
  }
  pbU = b.U().begin();  
  pby = b.y().begin();
  // skip first uncomplete patterns
  for (size_t row = 0; row < (max_order - 1); ++row) {
    ++pbU;
    ++pby;
  }
  for (size_t row = 0; row < n_patterns_net; ++row) {
    trainingfile_norm << *pbU << *pby << endl;
    if ((a.blocksize() > 0) && ((row+1) % a.blocksize() == 0)) {
      trainingfile_norm << endl;
    }
    if (! GLOBAL::denormalize) {
      //#WIN2017 vector<Real>::const_iterator pbu = pbU->begin();
      Uvector::const_iterator pbu = pbU->begin();
      vector<Real>::const_iterator pf = scale_factor_dynamic.begin();
      vector<Real>::const_iterator ps = scale_shift_dynamic.begin();
      while (pbu != pbU->end()) {
	trainingfile_orig << denormalization(*(pbu++), *(pf++), *(ps++)) 
			  << " ";
      }
      trainingfile_orig << denormalization(*pby, *pf, *ps) << endl;
      if ((a.blocksize() > 0) && ((row+1) % a.blocksize() == 0)) {
	trainingfile_orig << endl;
      }
    }
    ++pbU;
    ++pby;
  }
  trainingfile_norm.close();
  if (! GLOBAL::denormalize) {
    trainingfile_orig.close();
  }
  return;
}

