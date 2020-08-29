#ifndef GLOBAL_HH
#define GLOBAL_HH

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
 * $Id: global.hh,v 2.9 2020-08-29 10:47:39+00:00:02 manfred Exp $
 */



#ifndef WIN2017
#include <string>
#include <fstream.h>
#include <vector.h>  // STL vectors
#include <vec.h>     // TNT vectors
#include <assert.h>
#else
#include <string>
#include <cstring>
#include <fstream>
#include <vector>    // STL vectors
#include <tnt.h>     // TNT vectors
#include <assert.h>
#endif

#define _LIMITS_H    // avoid name conflicts with /usr/include/limits.h
#include <float.h>

#define DO_NOT_USE_STRINGSTREAM
#ifndef DO_NOT_USE_STRINGSTREAM
#include <sstream>
#else
#include <stdio.h>
#endif


using namespace std;
using namespace TNT;


/* Macro for debugging (control outputs)
 */
#define PRINT(X) cout<<flush; cerr<<"### "<< (#X) <<" = `"<<(X)<<"'\n"<<flush; 


/** @name #Real#
 * @type typedef
 * 
 * The type #Real# is determined by the definition of
 * either #LONGDOUBLE#, #DOUBLE#, or #FLOAT#. The constants
 * #REAL_MAX#, #REAL_MIN#, #REAL_EPSILON#, and #REAL_NAME#
 * are then set accordingly to the type of #Real#.
 */
#if defined(LONGDOUBLE)
// use long double and exact sigmoid function
typedef long double Real;
#define EXACT_SIGMOID
#define REAL_MAX LDBL_MAX
#define REAL_MIN LDBL_MIN
#define REAL_EPSILON LDBL_EPSILON
#define REAL_DIG LDBL_DIG
#define REAL_NAME "long double"
#elif defined(DOUBLE)
// use double
typedef double Real;
#define REAL_MAX DBL_MAX
#define REAL_MIN DBL_MIN
#define REAL_EPSILON DBL_EPSILON
#define REAL_DIG DBL_DIG
#define REAL_NAME "double"
#else
// use float
typedef float Real;
#define REAL_MAX FLT_MAX
#define REAL_MIN FLT_MIN
#define REAL_EPSILON FLT_EPSILON
#define REAL_DIG FLT_DIG
#define REAL_NAME "float"
#endif


extern const char* file_separator;

/** @name #mode_type#
 * @type enum
 *
 * Mode enumeration:
 * UNDEFD_MODE == undefined mode; (initial value)
 * MODELING == build fuzzy model based on training and validation data
 * ESTIMATION == one-step prediction using data from file
 * SIMULATION == multiple step prediction using data from file
 * PRINST_SETS == print LaTeX and graphical descrition of a fuzzy model
 * MAKE_DATA == prepare data for fuzzy modeling
 */
enum mode_type { UNDEFD_MODE, MODELING, ESTIMATION, SIMULATION, PRINT_SETS,
		 MAKE_DATA, NORMALIZE };

enum algo_type { UNDEFD_ALGO, RPROP, GRAD_DESCENT, HOOKE_JEEVES, ROSENBROCK };

/** @name Global constants
 */
//@{
/// prefix of fuzzy model output file
extern const char* model_prefix;
/// prefix of pixel output file
extern const char* fzy2pixl_prefix;
/// prefix of data generation file
extern const char* data_prefix;
/// prefix of estimation output file
extern const char* estimation_prefix;
/// prefix of simulation output file
extern const char* simulation_prefix;
/// suffix of fuzzy model output file
extern const char* model_suffix;
/// suffix of r2 output file
extern const char* r2_suffix;
/// suffix of error output file
extern const char* error_suffix;
/// suffix of logfile
extern const char* log_suffix;
/// suffix of output file
extern const char* output_suffix;
/// suffix of pixel file (for graphical description of a fuzzy set)
extern const char* pixl_suffix;
/// suffix of eps file (graphical eps description of a fuzzy set)
extern const char* eps_suffix;
/// suffix of LaTeX file (LaTeX description of a fuzzy model)
extern const char* tex_suffix;
/// software release number
extern const char* release;
/// software release date
extern const char* release_date;
//@}


/** @name Fuzzy Set constants
 * Constants for Fuzzy Sets.
 * @memo
 */
//@{
/// definition of right border of input space
const Real mu_left = 0.5;    
/// definition of left border of input space
const Real mu_right = -0.5;  
// const Real mu_left = 0.6;
// const Real mu_right = -0.6;
/// initialize consequence parameters c
const Real cons_0 = 0.0;
/// initialize delta $\mu$
const Real delta_mu_0 = (mu_left - mu_right) * 0.001;
/// initialize delta c 
const Real delta_cons_0 = 0.001;
#ifdef TRAPEZOIDAL_FSETS
/// initialize $\sigma$
const Real sigma_0 = 2.0;
/// initialize delta $\sigma$
const Real delta_sigma_0 = 0.02;
#else
const Real sigma_0 = 10.0;
const Real delta_sigma_0 = 0.1;
#endif

/// upper border of delta $\mu$ 
const Real max_delta_mu = 0.01;
/// upper border of delta c 
const Real max_delta_cons = 0.01;
/// upper border of delta $\sigma$ 
const Real max_delta_sigma = 1.0;
/// lower border of delta $\mu$ 
const Real min_delta_mu = 0.00001;
/// lower border of delta c 
const Real min_delta_cons = 0.00001;
/// lower border of delta $\sigma$ 
const Real min_delta_sigma = 0.001;

/// upper border of $\sigma$
const Real max_sigma_init = 200.0;
/// lower border of $\sigma$
const Real min_sigma_init = REAL_MIN;

/// RPROP step lengthening for successful step
const Real eta_plus = 1.2;
/// RPROP step shortening for unsuccessful step
const Real eta_minus = 0.5;
//@}


/** @name Global variables
 * @type
 */
namespace GLOBAL {
  /** @name Options and settings for all modes
   */
  //@{
  /// the mode (modeling, estimation, simulation, etc.)
  extern mode_type mode;
  /// program name
  extern char* prgname;
  /// binary path
  extern char* binpath;
  /// trace level
  extern int tracelevel;
  /// verbose level
  extern int verbose;
  /// inhibit verbose output
  extern int quiet;
  /// basic filename of output files
  extern string basefilename;
  /// output files' filename extension
  extern string filename_extension;
  /// name of trace file
  extern string tracefilename;
  /// trace log stream
  extern ofstream tracefile;
  /// name of verbose log file
  extern string logfilename;
  /// verbose log stream
  extern ofstream logfile;
  /// take back normalization 
  extern int denormalize;
  //@}

  /** @name Options for #mode == MODELING#
   */
  //@{
  /// optimization algorithm
  extern algo_type optimization;
  /// dimension of fuzzy rules' consequences 
  extern size_t consequence_dimension;
  /// error norm (e.g., 1 == L1-norm, 2 == L2-norm)
  extern int norm;
  /// local consequence optimization
  extern int local_cons_optimization;
  /// use shortcut for Kang's heuristic search
  extern int shortcut;
  /// reset consequence parameters 
  extern int reset_cons;
  /// update premise when optimizing 
  extern int update_premise;
  /// limit no. of fuzzy rules
  extern size_t max_n_rules;
  /// minimal no. of fuzzy rules
  extern size_t min_n_rules;
  /// same mu of adjacent fuzzy sets (i.e., input space divisions)
  extern int adjacent_equal_mu;
  /// take mean of several optimization iterations before validation  
  extern size_t steps_per_validation;
  /// optimize the consequence of each new candidate model by SVD
  extern int consequence_optimize_SVD;
  /// maximal no. of optimization iterations for parallel tuning
  extern size_t max_opt_iterations_parallel;
  /// minimal no. of optimization iterations
  extern size_t min_opt_iterations;
  /// maximal no. of optimization iterations
  extern size_t max_opt_iterations;
  /// maximal sigma of fuzzy sets
  extern Real max_sigma;
  /// minimal sigma of fuzzy sets
  extern Real min_sigma;
  /// terminate modeling when R2 improvement is below 
  extern Real R2_improvement;
  /// terminate modeling when best model's R2 improvement is below
  extern Real best_R2_improvement;
  /// no. of additional optimization iterations at each epoch's best model
  extern int optimize_epoch_best;
  /// no. of additional optimization iterations at the globally best model
  extern int optimize_global_best;
  /// gradient descent learning rate
  extern Real alpha;
  /// gradient descent momentum
  extern Real beta;
  /// algorithm for parameter optimization (fine tuning) using parallel model
  extern algo_type parallel_optimization;
  //@}
  
  /** @name Options for #mode == PRINT_SETS#
   */
  //@{
  /// no. of interpolation pixels for fuzzy sets' graphical output
  extern size_t n_pixels;
  /// normalize each fset value as F(x)/(sum Fi(x)) 
  extern int global_fset_value;
  /// scale of y-axis
  extern int scale;
  /// precision of numbers to beeing output in the TeX file
  extern int tex_precision;
  /// execute gnuplot system call; 0==no, 1==yes
  extern int execute_gnuplot_system_call;
  /// gnuplot script file extension, usually ".gpl"
  extern string gnuplot_file_extension;
  //@}
  
  /** @name Options for #mode == ESTIMATION or SIMULATION#.
   */
  //@{
  /// error offset (only added when printing the RMS into a file)
  extern Real error_offset;
  /// perform Page-Hinkley test: first column increase, second decrease
  extern int page_hinkley;
  /// mean value
  extern Real page_hinkley_mu_0;
  /// increase step to detect
  extern Real page_hinkley_nu_inc;
  /// decrease step to detect
  extern Real page_hinkley_nu_dec;
  /// sensitivity lambda
  extern Real page_hinkley_lambda;
  //@}

  /** @name Options for #mode == SIMULATION#.
   */
  //@{
  /// order of y (y-1, y-2, ..., y-order)
  extern int order;
  //@}

  /** @name Options for #mode == MAKE_DATA#.
   */
  //@{
  /// no. of learning/training patterns
  extern int n_training_data;
  /// no. patterns to skip at the beginning
  extern int n_skip_data;
  /// dynamic orders of u and y 
  extern vector<int> dynamic_orders;
  /// start with u(t) as regressor (instead of u(t-1))
  extern int ut_regressor;
  //@}

  /** @name Options for #mode == NORMALIZE#.
   */
  //@{
  /// compute scale by using only some data, instead of by the whole set
  extern int scale_only_used_data;
  /// scale by standard deviation (default)
  extern int scale_by_standard_deviation;
  /// scale by range
  extern int scale_by_range;
  /// scale by (sigma_factor * stddev)
  extern Real sigma_factor;
  //@}

}


/** @name Help functions
 * @memo  
 */
//@{
/** print trace information
 * @memo  
 */
template <class T>
void tracemsg(int level, const string& name, const T& object) {
  if (GLOBAL::tracelevel >= level) {
	cerr << "*** " << name << " = `" << object << "'\n" << flush; 	
    if (GLOBAL::tracefile) {
      GLOBAL::tracefile << "***trace: " << name << " = `" << object << "'\n" << flush; 
    }
  }
}

/** print verbose information
 * @memo  
 */
template <class T>
void verbose(int level, const string& name, const T& object) {
  if (GLOBAL::verbose > level) {
    if (! GLOBAL::quiet) {
      cout << name << " = `" << object << "'\n"; 
    }
    if (GLOBAL::logfile) {
      GLOBAL::logfile << name << " = `" << object << "'\n"; 
    }
  }
}

/** signum function
 * @memo  
 */
inline Real sign(Real x) {
  if (x > 0.0) {
    return 1.0;
  }
  else if (x < 0.0) {
    return -1.0;
  }
  return 0.0;
}

/** integer to string
 * @memo  
 */
string itos(int i);

/** double to string
 * @memo  
 */
string dtos(double r);

/** double to string
 * @memo  
 */
string dtos2(double r);

/* normalize */
inline Real normalization(Real u, Real factor, Real shift) {
  return ((u - shift) * factor);
}

/* take normalization back */
inline Real denormalization(Real u, Real factor, Real shift) {
  if (factor == 0.0) {
    return REAL_MAX;
  }
  return ((u / factor) + shift);
}

//@}


/** @name Error and exception handling
 */
//@{ 
/** Error base class.
 * @memo
 */
class Error {
protected:
  string errormessage;
public:
  Error(const string& errmsg) : errormessage(errmsg) {}
  virtual ~Error() { }
  virtual const string& msg() const { return errormessage; }
};

/** Error when opening a file or file not present.
 * @memo
 */
class FileOpenError : public Error {
public:
  FileOpenError(const string& errmsg) 
    : Error("cannot open file `"+errmsg+"'!") {}
};

/** Input value found that is not covered by a model's rule.
 * @memo
 */
class IncompleteCoverageError : public Error {
public:
  IncompleteCoverageError(const string& errmsg)
    : Error("`sum_w == 0' occured in `"+errmsg+"'!") {}
};

/** Invalid consequence dimension.
 * @memo
 */
class ConsdimError : public Error {
public:
  ConsdimError(const string& msg1, const string& msg2)
    : Error("invalid cdim `"+msg1+"'! 1 <= cdim <= "+msg2+" required.") {}
};

/** Parse error when loading a fuzzy model from file.
 * @memo
 */
class ParseError : public Error {
public:
  ParseError(int n, const string& msg1, const string& msg2)
    : Error(itos(n) + ": " + msg1 + " at `" + msg2 + "'") {}
};

/** System call was unsuccessful.
 * @memo
 */
class SystemCallError : public Error {
public:
  SystemCallError(const string& msg)
    : Error("unsuccessful system call `" + msg + "'") {}
};

/** Singular Value Decomposition (SVD) unsuccessful.
 * @memo
 */
class SVDError : public Error {
public:
  SVDError() 
    : Error("SVD(): m < n! Number of rows less than number of columns!") {}
};

/** Input value found where function is not defined
 * @memo
 */
class FunctionUndefinedError : public Error {
public:
  FunctionUndefinedError (const string& errmsg)
    : Error("function undefined in `"+errmsg+"'!") {}
};

//@}

#endif /// #ifndef GLOBAL_HH
