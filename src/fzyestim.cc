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
 * $Id: fzyestim.cc,v 2.9 2020-08-29 10:47:37+00:00:02 manfred Exp $
 */


#include <time.h>

#include "global.hh"
#include "fmodel.hh"
#include "data.hh"

#include "fzyestim.hh"


void fzyestim(char* fzyfilename, char* datafilename) throw (Error) {

  // load fuzzy model
  FModel fmodel;
  fmodel.load(fzyfilename);
  if ( (GLOBAL::verbose > 1) && (! GLOBAL::quiet)) {
    cout << "loaded model:\n\n" << fmodel << endl;
  }
  if (fmodel.rdim() < 1) {
    string msg = "invalid fmodel (rdim < 1) loaded from file";
    throw(Error(msg));
  }
  if (fmodel.cdim() < 1) {
    string msg = "invalid fmodel (cdim < 1) loaded from file";
    throw(Error(msg));
  }
  if (fmodel.udim() < 1) {
    string msg = "invalid fmodel (udim < 1) loaded from file";
    throw(Error(msg));
  }
  if (fmodel.cdim() > fmodel.udim() + 1) {
    string msg = "invalid fmodel (cdim > udim+1) loaded from file";
    throw(Error(msg));
  }
  if (fmodel.sdim() != 2 * (fmodel.rdim() - 1)) {
    string msg = "invalid fmodel (sdim != 2(rdim-1)) loaded from file";
    throw(Error(msg));
  }

  // load data
  Data data;
  data.load(datafilename);
  if (data.U().size() < 1) {
    string msg = "loaded data file contains no values!";
    throw(Error(msg));
  }

  // check if model fits to data
  if (GLOBAL::consequence_dimension > 1 + data.udim()) {
    throw(ConsdimError(itos(GLOBAL::consequence_dimension), 
		       itos(1 + data.udim())));
  }
  if (fmodel.udim() != data.udim()) {
    string msg = "loded model has another udim than loaded data";
    throw(Error(msg));
  }


// ////////////////////////////////////////////////////////////////////

  /* time measurements
   * uncomment this block to get time measurement information 
   * when calling the estimation 
   */ 

  /*
    {
      const int n_iterations = 10000;
      time_t t1;
      time_t t2;
      int tdiff;
      for (int k=0; k<1000; ++k) {
	fmodel.estimation(data);
      }

      t1 = time(NULL);
      for (int k=0; k<n_iterations; ++k) {
	fmodel.estimation(data);
      }
      t2 = time(NULL);
      tdiff = t2 - t1;
      PRINT(tdiff);

      t1 = time(NULL);
      for (int k=0; k<n_iterations; ++k) {
	fmodel.RPROP(data);
      }
      t2 = time(NULL);
      tdiff = t2 - t1;
      PRINT(tdiff);
    }
  */

// ////////////////////////////////////////////////////////////////////

  // ok, do the job
  string basefilename;
  if (GLOBAL::mode == ESTIMATION) {
    basefilename = (string)estimation_prefix + GLOBAL::filename_extension;
  }
  else if (GLOBAL::mode == SIMULATION) {
    basefilename = (string)simulation_prefix + GLOBAL::filename_extension;
  }
  else {
    assert(1 == 0);
  }
  int suffix_len = strlen(model_suffix);
  int prefix_len = strlen(model_prefix);
  int length = strlen(fzyfilename) - suffix_len - prefix_len;
  if (length < 1) {
    basefilename += fzyfilename;
  } 
  else {
    basefilename += ((string)fzyfilename).substr(prefix_len, length);
  }
  // basefilename += GLOBAL::filename_extension;
  string outfilename = basefilename + output_suffix;
  string r2filename = basefilename + r2_suffix;
  string errfilename = basefilename + error_suffix;
  ofstream r2file(r2filename.c_str());
  if (!r2file) {
    throw FileOpenError(r2filename);
  }
  ofstream errfile(errfilename.c_str());
  if (!errfile) {
    throw FileOpenError(errfilename);
  }
  Real error = -1.0;
  if (GLOBAL::mode == ESTIMATION) {
    error = fmodel.estimation(data, outfilename.c_str());
  }
  else if (GLOBAL::mode == SIMULATION) {
    error = fmodel.simulation(data, outfilename.c_str());
  }
  Real R2 = fmodel.R2(data);
  verbose(1, "error", error);
  verbose(1, "R2", R2);
  if (GLOBAL::mode == ESTIMATION) {
    r2file << "### estimation data: \"" << datafilename << "\"\n";
    r2file << R2 << endl;
    errfile << "### estimation data: \"" << datafilename << "\"\n";
    errfile << error << endl;
  }
  else if (GLOBAL::mode == SIMULATION) {
    r2file << "### simulation data: \"" << datafilename << "\"\n";
    r2file << R2 << endl;
    errfile << "### simulation data: \"" << datafilename << "\"\n";
    errfile << error << endl;
  }
  r2file.close();
  errfile.close();
  if ((GLOBAL::verbose > 0) && (! GLOBAL::quiet)) {
    cout << "R2 and error from ";
    if (GLOBAL::mode == ESTIMATION) {
      cout << "estimation ";
    }
    if (GLOBAL::mode == SIMULATION) {
      cout << "simulation ";
    }
    cout << "for `" << fzyfilename << "': " 
	 << R2 << " \t" << error << endl;
  }
  return;
}

