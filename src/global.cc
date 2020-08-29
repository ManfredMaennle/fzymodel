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
 * $Id: global.cc,v 2.9 2020-08-29 10:47:38+00:00:02 manfred Exp $
 */


#include "global.hh"


/*
 * ********** definition of global constants
 */

#ifndef WIN2017
const char* file_separator = "/";
#else
const char* file_separator = "\\";
#endif


const char* model_prefix = "mod_";
const char* fzy2pixl_prefix = "pxl_";
const char* data_prefix = "dat_";
const char* estimation_prefix = "est_";
const char* simulation_prefix = "sim_";
const char* model_suffix = ".fzy";
const char* r2_suffix = ".r2";
const char* error_suffix = ".err";
const char* log_suffix = ".log";
const char* output_suffix = ".out";
const char* pixl_suffix = ".pxl";
const char* eps_suffix = ".eps";
const char* tex_suffix = ".tex";

//#WIN2017 const char* release = "2.6";
//#WIN2017 const char* release_date = "12 October 2000";
const char* release = "2.9";
const char* release_date = "29 July 2020";
 

/*
 * ********** definition of global variables
 */

// the mode
mode_type GLOBAL::mode = UNDEFD_MODE;

// options and settings for all modes
char* GLOBAL::prgname = (char*) "";
//#WIN2017 char* GLOBAL::binpath = "/home/i8fs1/maennle/local/bin/";
char* GLOBAL::binpath = (char*) "";
int GLOBAL::tracelevel = 0;
int GLOBAL::verbose = 2;
int GLOBAL::quiet = 0;
string GLOBAL::basefilename;
string GLOBAL::filename_extension = "";
string GLOBAL::logfilename;
ofstream GLOBAL::logfile;
string GLOBAL::tracefilename = "tracefile.log";
ofstream GLOBAL::tracefile;
int GLOBAL::denormalize = 0;

// mode == MODELING
algo_type GLOBAL::optimization = RPROP;
size_t GLOBAL::consequence_dimension = 1;
int GLOBAL::norm = 2;
int GLOBAL::local_cons_optimization = 0;
int GLOBAL::shortcut = 0;
int GLOBAL::reset_cons = 0;
int GLOBAL::update_premise = 1;
size_t GLOBAL::min_n_rules = 2;
size_t GLOBAL::max_n_rules = 9;
int GLOBAL::adjacent_equal_mu = 0;
size_t GLOBAL::steps_per_validation = 4;
//size_t GLOBAL::steps_per_validation = 1;
int GLOBAL::consequence_optimize_SVD = 0;
//int GLOBAL::consequence_optimize_SVD = 1;
size_t GLOBAL::max_opt_iterations_parallel = 100000;
size_t GLOBAL::min_opt_iterations = 12;
size_t GLOBAL::max_opt_iterations = 250;
//size_t GLOBAL::max_opt_iterations = 800;
Real GLOBAL::max_sigma = max_sigma_init;
Real GLOBAL::min_sigma = min_sigma_init;
Real GLOBAL::R2_improvement = 0.0;
Real GLOBAL::best_R2_improvement = 1e-4;
int GLOBAL::optimize_epoch_best = 0;
int GLOBAL::optimize_global_best = 10000;
//int GLOBAL::optimize_global_best = 0;
Real GLOBAL::alpha = 0.001;
Real GLOBAL::beta = 0.9;
algo_type GLOBAL::parallel_optimization = UNDEFD_ALGO;

// mode == PRINT_SETS
size_t GLOBAL::n_pixels = 100;
int GLOBAL::global_fset_value = 0;
int GLOBAL::scale = 0;
int GLOBAL::tex_precision = 3;
int GLOBAL::execute_gnuplot_system_call = 1;
string GLOBAL::gnuplot_file_extension = ".gpl";
 
// mode == ESTIMATION or SIMULATION
Real GLOBAL::error_offset = 0.0;
int GLOBAL::page_hinkley = 0;
Real GLOBAL::page_hinkley_mu_0 = 0.0;
Real GLOBAL::page_hinkley_nu_inc = 0.0;
Real GLOBAL::page_hinkley_nu_dec = 0.0;
Real GLOBAL::page_hinkley_lambda = 0.0;

// mode == SIMULATION
int GLOBAL::order = 0;

// mode == MAKE_DATA
int GLOBAL::n_training_data = 0;
int GLOBAL::n_skip_data = 0;
vector<int> GLOBAL::dynamic_orders;
int GLOBAL::ut_regressor = 0;

// mode == NORMALIZE
int GLOBAL::scale_only_used_data = 0;
int GLOBAL::scale_by_standard_deviation = 1;
int GLOBAL::scale_by_range = 0;
/// factor for normalization by standard deviation (sigma)
Real GLOBAL::sigma_factor = 1.64;   // 90 % of data in +- sigma_factor * sigma
//Real GLOBAL::sigma_factor = 1.96; // 95 % of data in +- sigma_factor * sigma
//Real GLOBAL::sigma_factor = 2.58; // 99 % of data in +- sigma_factor * sigma


/*
 * help functions
 */

string itos(int i) {
#ifndef DO_NOT_USE_STRINGSTREAM
      ostringstream valuestrm;
      valuestrm << i;
      return valuestrm.str();
#else
      char valuebuffer[1000];
      sprintf(valuebuffer, "%d", i);
      return (string)valuebuffer;
#endif
}


string dtos(double x) {
#ifndef DO_NOT_USE_STRINGSTREAM
      ostringstream valuestrm;
      valuestrm << x;
      return valuestrm.str();
#else
      char valuebuffer[1000];
      sprintf(valuebuffer, "%g", x);
      return (string)valuebuffer;
#endif
}

string dtos2(double x) {
#ifndef DO_NOT_USE_STRINGSTREAM
      ostringstream valuestrm;
      int prior_precision = valuestrm.precision();
      valuestrm.precision(3);
      valuestrm << x;
      valuestrm.precision(prior_precision);      
      return valuestrm.str();
#else
      char valuebuffer[1000];
      sprintf(valuebuffer, "%1.3g", x);
      return (string)valuebuffer;
#endif
}

