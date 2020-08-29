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
 * $Id: main.cc,v 2.9 2020-08-29 10:47:39+00:00:02 manfred Exp $
 */


#include "global.hh"
#include "data.hh"
#include "param.hh"
#include "fmodel.hh"
#include "main.hh"

#include "fzymodel.hh"
#include "fzyestim.hh"
#include "fzy2sets.hh"
#include "fzymkdat.hh"
#include "fzynorml.hh"

/*
 * ********** local types and variables
 */


/*
 * ********** local functions
 */

void exit_with_help(std::ostream&);
void exit_on_msg(std::ostream&, const string& message);
void exit_on_error(std::ostream& strm, const string& msg);


/*
 * ********** main(), local, and global functions
 */

int 
main(int argc, char* argv[], char* env[])
{
  char* infilename1 = NULL;
  char* infilename2 = NULL;
  char* infilename3 = NULL;
  GLOBAL::tracefile.open(GLOBAL::tracefilename.c_str());
  tracemsg(1, "in main(): opened trace file", GLOBAL::tracefilename.c_str());
  //GLOBAL::logfile.open(GLOBAL::logfilename.c_str());
 

  // /// determine mode
  {
    GLOBAL::prgname = argv[0];
    string modename = (string)argv[0];
    char* last_token = NULL;
    char* token = (char *)modename.c_str();
    token = strtok(token, file_separator);
    while (token != NULL) {
      last_token = token;
      token = strtok(NULL, file_separator);
    }
	tracemsg(1, "in main(): determine mode, last_token", last_token);

    if ((last_token == NULL) || (strlen(last_token) < 8)) {
      string msg = (string)argv[0] + ": fatal error: wrong program name!";
      exit_on_error(cerr, msg);
    }

    if (strncmp(last_token, "fzymodel", 8) == 0) {
      GLOBAL::mode = MODELING;
      GLOBAL::prgname = (char*)"fzymodel";
    }
    else if (strncmp(last_token, "fzyestim", 8) == 0) {
      GLOBAL::mode = ESTIMATION;
      GLOBAL::prgname = (char*)"fzyestim";
    }
    else if (strncmp(last_token, "fzysimul", 8) == 0) {
      GLOBAL::mode = SIMULATION;
      GLOBAL::prgname = (char*)"fzysimul";
    }
    else if (strncmp(last_token, "fzy2sets", 8) == 0) {
      GLOBAL::mode = PRINT_SETS;
      GLOBAL::prgname = (char*)"fzy2sets";
    }
    else if (strncmp(last_token, "fzymkdat", 8) == 0) {
      GLOBAL::mode = MAKE_DATA;
      GLOBAL::prgname = (char*)"fzymkdat";
    }
    else if (strncmp(last_token, "fzynorml", 8) == 0) {
      GLOBAL::mode = NORMALIZE;
      GLOBAL::prgname = (char*)"fzynorml";
    }
    else {
	  tracemsg(1, "in main(): determine mode, mode not found error: last_token", last_token);
      string msg = (string)argv[0] + ": fatal error: invalid program name: " + last_token;
      exit_on_error(cerr, msg);
    }
  }
  tracemsg(1, "in main(): resulting mode: GLOBAL::prgname", GLOBAL::prgname);

  // /// scan argument vector
  int i = 0;
  if (argc < 2) {
      exit_with_help(cerr); 
  }
  while ( ++i < argc ) {
    string arg = argv[i];
    if (! arg.compare("-h")) { 
      exit_with_help(cerr); 
    }
    else if (! arg.compare("-f1")) { 
      if (++i < argc) {
	infilename1 = argv[i]; 
      }
      else {
	exit_on_msg(cerr, "error: no argument for option -f1 given!");
      }
    }
    else if (! arg.compare("-f2")) { 
      if ((GLOBAL::mode == MODELING) 
	  || (GLOBAL::mode == ESTIMATION)
	  || (GLOBAL::mode == SIMULATION)
	  || (GLOBAL::mode == MAKE_DATA)
	  || (GLOBAL::mode == NORMALIZE)) {
	if (++i < argc) {
	  infilename2 = argv[i]; 
	}
	else {
	  exit_on_msg(cerr, "error: no argument for option -f2 given!");
	}
      }
      else {
	exit_on_msg(cerr, "unknown option `" + arg + "'");
      }
    }
    else if (! arg.compare("-f3")) { 
      if (GLOBAL::mode == MAKE_DATA) {
	if (++i < argc) {
	  infilename3 = argv[i]; 
	}
	else {
	  exit_on_msg(cerr, "error: no argument for option -f3 given!");
	}
      }
      else {
	exit_on_msg(cerr, "unknown option `" + arg + "'");
      }
    }
    else if (! arg.compare("-e")) { 
      if (++i < argc) {
	GLOBAL::filename_extension = (string)argv[i]; 
      }
      else {
	exit_on_msg(cerr, "error: no argument for option -e given!");
      }
    }
    else if (! arg.compare("-c")) { 
      if (GLOBAL::mode == MODELING) {
	if (++i < argc) {
	  GLOBAL::consequence_dimension = atoi(argv[i]);
	}
	else {
	  exit_on_msg(cerr, "error: no argument for option -c given!");
	}
      }
      else {
	exit_on_msg(cerr, "unknown option `" + arg + "'");
      }
    }
    else if (! arg.compare("-g")) { 
      if (GLOBAL::mode == MODELING) {
	GLOBAL::optimization = GRAD_DESCENT;
	if ((i < argc-1) && (argv[i+1][0] != '-')) {
	  GLOBAL::alpha = atof(argv[++i]);
	}
	if ((i < argc-1) && (argv[i+1][0] != '-')) {
	  GLOBAL::beta = atof(argv[++i]);
	}
      }
      else {
	exit_on_msg(cerr, "unknown option `" + arg + "'");
      }
    }
    else if (! arg.compare("-n")) { 
      if ((GLOBAL::mode == MODELING) 
	  || (GLOBAL::mode == ESTIMATION)
	  || (GLOBAL::mode == SIMULATION)) {
	GLOBAL::denormalize = 1; 
      }
      else if (GLOBAL::mode == PRINT_SETS) {
	GLOBAL::denormalize = 1; 
	if (++i < argc) {
	  infilename2 = argv[i]; 
	}
	else {
	  exit_on_msg(cerr, "error: no argument for option -n given!");
	}
      }
      else {
	exit_on_msg(cerr, "unknown option `" + arg + "'");
      }
    }
    else if (! arg.compare("-r")) { 
      if (GLOBAL::mode == MODELING) {
	if (++i < argc) {
	  GLOBAL::min_n_rules = atoi(argv[i]);
	}
	else {
	  exit_on_msg(cerr, "error: no argument for option -r given!");
	}
      }
      else {
	exit_on_msg(cerr, "unknown option `" + arg + "'");
      }
    }
    else if (! arg.compare("-R")) { 
      if (GLOBAL::mode == MODELING) {
	if (++i < argc) {
	  GLOBAL::max_n_rules = atoi(argv[i]);
	}
	else {
	  exit_on_msg(cerr, "error: no argument for option -R given!");
	}
      }
      else{
	exit_on_msg(cerr, "unknown option `" + arg + "'");
      }
    }
    else if (! arg.compare("-pO")) { 
      if (GLOBAL::mode == MODELING) {
	if (++i < argc) {
	  GLOBAL::max_opt_iterations_parallel = atoi(argv[i]);
	}
	else {
	  exit_on_msg(cerr, "error: no argument for option -pO given!");
	}
      }
      else {
	exit_on_msg(cerr, "unknown option `" + arg + "'");
      }
    }
    else if (! arg.compare("-P")) { 
      if (GLOBAL::mode == MODELING) {
	GLOBAL::update_premise = 0;
      }
      else {
	exit_on_msg(cerr, "unknown option `" + arg + "'");
      }
    }
    else if (! arg.compare("-C")) { 
      if (GLOBAL::mode == MODELING) {
	GLOBAL::consequence_optimize_SVD = 1;
      }
      else {
	exit_on_msg(cerr, "unknown option `" + arg + "'");
      }
    }
    else if (! arg.compare("-o")) { 
      if (GLOBAL::mode == MODELING) {
	if (++i < argc) {
	  GLOBAL::min_opt_iterations = atoi(argv[i]);
	}
	else {
	  exit_on_msg(cerr, "error: no argument for option -o given!");
	}
      }
      else if (GLOBAL::mode == MAKE_DATA) {
	GLOBAL::denormalize = 1; 
      }
      else {
	exit_on_msg(cerr, "unknown option `" + arg + "'");
      }
    }
    else if (! arg.compare("-O")) { 
      if (GLOBAL::mode == MODELING) {
	if (++i < argc) {
	  GLOBAL::max_opt_iterations = atoi(argv[i]);
	}
	else {
	  exit_on_msg(cerr, "error: no argument for option -O given!");
	}
      }
      else {
	exit_on_msg(cerr, "unknown option `" + arg + "'");
      }
    }
    else if (! arg.compare("-i")) { 
      if (GLOBAL::mode == MODELING) {
	if (++i < argc) {
	  GLOBAL::R2_improvement = atof(argv[i]);
	}
	else {
	  exit_on_msg(cerr, "error: no argument for option -i given!");
	}
      }
      else {
	exit_on_msg(cerr, "unknown option `" + arg + "'");
      }
    }
    else if (! arg.compare("-I")) { 
      if (GLOBAL::mode == MODELING) {
	if (++i < argc) {
	  GLOBAL::best_R2_improvement = atof(argv[i]);
	}
	else {
	  exit_on_msg(cerr, "error: no argument for option -I given!");
	}
      }
      else {
	exit_on_msg(cerr, "unknown option `" + arg + "'");
      }
    }
    else if (! arg.compare("-p")) { 
      if (GLOBAL::mode == PRINT_SETS) {
	if (++i < argc) {
	  GLOBAL::n_pixels = atoi(argv[i]);
	}
	else {
	  exit_on_msg(cerr, "error: no argument for option -p given!");
	}
      }
      else {
	exit_on_msg(cerr, "unknown option `" + arg + "'");
      }
    }
    else if (! arg.compare("-t")) { 
      if (++i < argc) {
	GLOBAL::tracelevel = atoi(argv[i]);
      }
      else {
	exit_on_msg(cerr, "error: no argument for option -t given!");
      }
    }
    else if (! arg.compare("-v")) { 
      if (++i < argc) {
	GLOBAL::verbose = atoi(argv[i]);
      }
      else {
	exit_on_msg(cerr, "error: no argument for option -v given!");
      }
    }
    else if (! arg.compare("-q")) {
      GLOBAL::quiet = 1;
    }
    else if (! arg.compare("-s")) {
      if (GLOBAL::mode == PRINT_SETS) {
	GLOBAL::scale = 1;
      }
      else if (GLOBAL::mode == MODELING) {
	if (++i < argc) {
	  GLOBAL::steps_per_validation = atoi(argv[i]);
	}
	else {
	  exit_on_msg(cerr, "error: no argument for option -s given!");
	}
      }
      else {
	exit_on_msg(cerr, "unknown option `" + arg + "'");
      }
    }
    else if (! arg.compare("-l")) { 
      if (GLOBAL::mode == MODELING) {
	if (++i < argc) {
	  GLOBAL::local_cons_optimization = atoi(argv[i]);
	}
	else {
	  exit_on_msg(cerr, "error: no argument for option -l given!");
	}
      }
      else {
	exit_on_msg(cerr, "unknown option `" + arg + "'");
      }
    }
    else if (! arg.compare("-L")) { 
      if (GLOBAL::mode == MODELING) {
	if (++i < argc) {
	  GLOBAL::norm = atoi(argv[i]);
	}
	else {
	  exit_on_msg(cerr, "error: no argument for option -L given!");
	}
      }
      else {
	exit_on_msg(cerr, "unknown option `" + arg + "'");
      }
    }
    else if (! arg.compare("-Fs")) { 
      if (GLOBAL::mode == MODELING) {
	if (++i < argc) {
	  GLOBAL::min_sigma = atof(argv[i]);
	}
	else {
	  exit_on_msg(cerr, "error: no argument for option -Fs given!");
	}
      }
      else {
	exit_on_msg(cerr, "unknown option `" + arg + "'");
      }
    }
    else if (! arg.compare("-FS")) { 
      if (GLOBAL::mode == MODELING) {
	if (++i < argc) {
	  GLOBAL::max_sigma = atof(argv[i]);
	}
	else {
	  exit_on_msg(cerr, "error: no argument for option -FS given!");
	}
      }
      else {
	exit_on_msg(cerr, "unknown option `" + arg + "'");
      }
    }
    else if (! arg.compare("-am")) { 
      if (GLOBAL::mode == MODELING) {
	GLOBAL::adjacent_equal_mu = 1;
      }
      else {
	exit_on_msg(cerr, "unknown option `" + arg + "'");
      }
    }
    else if (! arg.compare("-ph")) { 
      if (GLOBAL::mode == MODELING) {
	GLOBAL::parallel_optimization = HOOKE_JEEVES;
	if (++i < argc) {
	  GLOBAL::order = atoi(argv[i]);
	}
	else {
	  exit_on_msg(cerr, "error: no argument for option -ph given!");
	}
      }
      else {
	exit_on_msg(cerr, "unknown option `" + arg + "'");
      }
    }
    else if (! arg.compare("-pr")) { 
      if (GLOBAL::mode == MODELING) {
	GLOBAL::parallel_optimization = ROSENBROCK;
	if (++i < argc) {
	  GLOBAL::order = atoi(argv[i]);
	}
	else {
	  exit_on_msg(cerr, "error: no argument for option -pr given!");
	}
      }
      else {
	exit_on_msg(cerr, "unknown option `" + arg + "'");
      }
    }
    else if (! arg.compare("-S")) { 
      if (GLOBAL::mode == MODELING) {
	if (++i < argc) {
	  GLOBAL::shortcut = atoi(argv[i]);
	}
	else {
	  exit_on_msg(cerr, "error: no argument for option -S given!");
	}
      }
      else {
	exit_on_msg(cerr, "unknown option `" + arg + "'");
      }
    }
    else if (! arg.compare("-d")) { 
      if (GLOBAL::mode == SIMULATION) {
	if (++i < argc) {
	  GLOBAL::order = atoi(argv[i]);
	}
	else {
	  exit_on_msg(cerr, "error: no argument for option -d given!");
	}
      }
      else if (GLOBAL::mode == MAKE_DATA) {
	while ( (++i < argc) && (argv[i][0] != '-') ) {
	  GLOBAL::dynamic_orders.push_back(atoi(argv[i]));
	}
	if ( (i < argc) && (argv[i][0] == '-') ) {
	  --i;
	}
      }
      else {
	exit_on_msg(cerr, "unknown option `" + arg + "'");
      }
    }
    else if (! arg.compare("-E")) { 
      if ((GLOBAL::mode == ESTIMATION) || (GLOBAL::mode == SIMULATION)) {
	if (++i < argc) {
	  GLOBAL::error_offset = atof(argv[i]);
	}
	else {
	  exit_on_msg(cerr, "error: no argument for option -E given!");
	}
      }
      else {
	exit_on_msg(cerr, "unknown option `" + arg + "'");
      }
    }
    else if (! arg.compare("-Dph")) { 
      if ((GLOBAL::mode == ESTIMATION) || (GLOBAL::mode == SIMULATION)) {
	GLOBAL::page_hinkley = 1;
	if (i+4 >= argc) {
	  exit_on_msg(cerr, 
		      "error: not enough arguments for option -Dph given!");
	}
	GLOBAL::page_hinkley_mu_0 = atof(argv[++i]);
	GLOBAL::page_hinkley_nu_inc = atof(argv[++i]);
	GLOBAL::page_hinkley_nu_dec = atof(argv[++i]);
	GLOBAL::page_hinkley_lambda = atof(argv[++i]);
      }
      else {
	exit_on_msg(cerr, "unknown option `" + arg + "'");
      }
    }
    else if (! arg.compare("-N")) { 
      if (GLOBAL::mode == PRINT_SETS) {
	GLOBAL::global_fset_value = 1;
      }
      else {
	exit_on_msg(cerr, "unknown option `" + arg + "'");
      }
    }
    else if (! arg.compare("-Np")) { 
      if (GLOBAL::mode == MAKE_DATA) {
	if (++i < argc) {
	  GLOBAL::n_training_data = atoi(argv[i]);
	}
	else {
	  exit_on_msg(cerr, "error: no argument for option -Np given!");
	}
      }
      else {
	exit_on_msg(cerr, "unknown option `" + arg + "'");
      }
    }
    else if (! arg.compare("-Ns")) { 
      if (GLOBAL::mode == MAKE_DATA) {
	if (++i < argc) {
	  GLOBAL::n_skip_data = atoi(argv[i]);
	}
	else {
	  exit_on_msg(cerr, "error: no argument for option -Ns given!");
	}
      }
      else {
	exit_on_msg(cerr, "unknown option `" + arg + "'");
      }
    }
    else if (! arg.compare("-sr")) { 
      if (GLOBAL::mode == NORMALIZE) {
	GLOBAL::scale_by_range = 1;
	GLOBAL::scale_by_standard_deviation = 0;
      }
      else {
	exit_on_msg(cerr, "unknown option `" + arg + "'");
      }
    }
    else if (! arg.compare("-sd")) { 
      if (GLOBAL::mode == NORMALIZE) {
	if (++i < argc) {
	  GLOBAL::scale_by_range = 0;
	  GLOBAL::scale_by_standard_deviation = 1;
	  GLOBAL::sigma_factor = atof(argv[i]);
	}
	else {
	  exit_on_msg(cerr, "error: no argument for option -sd given!");
	}
      }
      else {
	exit_on_msg(cerr, "unknown option `" + arg + "'");
      }
    }
    else if (! arg.compare("-u")) { 
      if (GLOBAL::mode == NORMALIZE) {
	if (++i < argc) {
	  GLOBAL::scale_only_used_data = atoi(argv[i]);
	}
	else {
	  exit_on_msg(cerr, "error: no argument for option -u given!");
	}
      }
      else {
	exit_on_msg(cerr, "unknown option `" + arg + "'");
      }
    }
    else if (! arg.compare("-rt")) { 
      if (GLOBAL::mode == MAKE_DATA) {
	GLOBAL::ut_regressor = 1;
      }
      else {
	exit_on_msg(cerr, "unknown option `" + arg + "'");
      }
    }
    else { // unknown option
      exit_on_msg(cerr, "unknown option `" + arg + "'");
    }
  } // end while ( argv[i] ) 

  
  // /// check options and settings
  if (GLOBAL::mode == MODELING) {
    if (infilename1 == NULL) {
      exit_on_msg(cerr, "error: no inputfile1 (learning data) given!");
    }
    if (infilename2 == NULL) {
      exit_on_msg(cerr, "error: no inputfile2 (validation data) given!");
    }
    if ( !(GLOBAL::consequence_dimension > 0)) {
      exit_on_msg(cerr, "error: argument at `-c' must be > 0!");
    }
    if ( !(GLOBAL::min_n_rules > 0)) {
      exit_on_msg(cerr, "error: argument at `-r' must be > 0!");
    }
    if ( !(GLOBAL::max_n_rules < 1000)) {
      exit_on_msg(cerr, "error: argument at `-r' must be < 1000!");
    }
    if ( !(GLOBAL::min_n_rules <= GLOBAL::max_n_rules)) {
      exit_on_msg(cerr,"error: argument at `-r' must be <= arg. at `-R'!");
    }
    if ( !(GLOBAL::min_opt_iterations > 0)) {
      exit_on_msg(cerr, "error: argument at `-o' must be > 0!");
    }
    if ( !(GLOBAL::max_opt_iterations < 100000)) {
      exit_on_msg(cerr, "error: argument at `-O' must be < 100000!");
    }
    if ( !(GLOBAL::min_opt_iterations <= GLOBAL::max_opt_iterations)) {
      exit_on_msg(cerr,"error: argument at `-o' must be <= arg. at `-O'!");
    }
    if ( !(GLOBAL::steps_per_validation > 0)) {
      exit_on_msg(cerr, "error: argument at `-s' must be > 0!");
    }
    if ( !(GLOBAL::steps_per_validation < 10000)) {
      exit_on_msg(cerr, "error: argument at `-s' must be < 10000!");
    }
    if ( !(GLOBAL::norm > 0)) {
      exit_on_msg(cerr, "error: argument at `-L' must be > 0!");
    }
    if ( !(GLOBAL::local_cons_optimization >= 0)) {
      exit_on_msg(cerr, "error: argument at `-l' must be >= 0!");
    }
    if ( !(GLOBAL::norm < 4)) {
      exit_on_msg(cerr, "error: argument at `-L' must be < 4!");
    }
  }
  else if (GLOBAL::mode == PRINT_SETS) {
    if (infilename1 == NULL) {
      exit_on_msg(cerr, "error: no inputfile1 (fuzzy model) given!");
    }
    if ( !(GLOBAL::n_pixels >= 10) ) {
      exit_on_msg(cerr, "error: argument at `-p' must be >= 10!");
    }
    if ( !(GLOBAL::n_pixels <= 1000000) ) {
      exit_on_msg(cerr, "error: argument at `-p' must be <= 1000000!");
    }
  }
  else if (GLOBAL::mode == ESTIMATION) {
    if (infilename1 == NULL) {
      exit_on_msg(cerr, "error: no inputfile1 (fuzzy model) given!");
    }
    if (infilename2 == NULL) {
      exit_on_msg(cerr, "error: no inputfile2 (data file) given!");
    }
  }
  else if (GLOBAL::mode == SIMULATION) {
    if (infilename1 == NULL) {
      exit_on_msg(cerr, "error: no inputfile1 (fuzzy model) given!");
    }
    if (infilename2 == NULL) {
      exit_on_msg(cerr, "error: no inputfile2 (data file) given!");
    }
    if ( !(GLOBAL::order > 0)) {
      exit_on_msg(cerr, "error: argument at `-o' must be > 0!");
    }
  }
  else if (GLOBAL::mode == MAKE_DATA) {
    if (infilename1 == NULL) {
      exit_on_msg(cerr, "error: no inputfile1 (normalization) given!");
    }
    if (infilename2 == NULL) {
      exit_on_msg(cerr, "error: no inputfile2 (input data file) given!");
    }
    if (infilename3 == NULL) {
      exit_on_msg(cerr, "error: no inputfile3 (output file) given!");
    }
    if (GLOBAL::n_training_data < 0) {
      exit_on_msg(cerr, "error: no. of training patterns < 0!");
    }
    if (GLOBAL::n_skip_data < 0) {
      exit_on_msg(cerr, "error: no. of patterns to skip < 0!");
    }
    vector<int>::const_iterator p = GLOBAL::dynamic_orders.begin();
    if (p == GLOBAL::dynamic_orders.end()) {
      exit_on_msg(cerr, "error: no data dynamics given (with -d option)!");
    }
    while (p != GLOBAL::dynamic_orders.end()) {
      if (*p < 0) {
	exit_on_msg(cerr, "error: dynamic < 1 at -d option");
      }
      ++p;
    }
  }
  else if (GLOBAL::mode == NORMALIZE) {
    if (infilename1 == NULL) {
      exit_on_msg(cerr, "error: no inputfile1 (input data) given!");
    }
    if (infilename2 == NULL) {
      exit_on_msg(cerr, "error: no inputfile2 (output file) given!");
    }
  }
  else {
    assert(1==0);
    string msg = (string)GLOBAL::prgname + ": fatal error: unknown mode!";
    exit_on_error(cerr, msg);
  }
  if (GLOBAL::page_hinkley) {
    if ( (GLOBAL::page_hinkley_nu_inc <= 0.0) 
	 || (GLOBAL::page_hinkley_nu_dec <= 0.0) 
	 || (GLOBAL::page_hinkley_lambda <= 0.0) ) {
      exit_on_msg(cerr, "error: mu, nu, or lambda <= 0 at -Dph option!");
    }
  }

  // /// ok, do the job

  try { // /// begin toplevel try
	tracemsg(10, "in main(): begin top level try", "");
    if (GLOBAL::mode == MODELING) {
      GLOBAL::basefilename = model_prefix + GLOBAL::filename_extension;
    }
    else if (GLOBAL::mode == PRINT_SETS) {
      GLOBAL::basefilename = fzy2pixl_prefix + GLOBAL::filename_extension;
    }
    else if (GLOBAL::mode == ESTIMATION) {
      GLOBAL::basefilename = estimation_prefix + GLOBAL::filename_extension;
    } 
    else if (GLOBAL::mode == SIMULATION) {
      GLOBAL::basefilename = simulation_prefix + GLOBAL::filename_extension;
    } 
    else if (GLOBAL::mode == MAKE_DATA) {
      GLOBAL::basefilename = data_prefix + GLOBAL::filename_extension;
    } 
    else if (GLOBAL::mode == NORMALIZE) {
      GLOBAL::basefilename = data_prefix + GLOBAL::filename_extension;
    } 
    else {
      assert(1==0);
    }

    if ( (GLOBAL::mode == MODELING)
	 || (GLOBAL::mode == PRINT_SETS) 
	 || (GLOBAL::mode == ESTIMATION)
	 || (GLOBAL::mode == SIMULATION) ) {
      GLOBAL::basefilename += (string)"c"+itos(GLOBAL::consequence_dimension);
      GLOBAL::logfilename = GLOBAL::basefilename + "_rr" + log_suffix;
    }
    else if (GLOBAL::mode == MAKE_DATA) {
      GLOBAL::logfilename = GLOBAL::basefilename + "create" + log_suffix;
    }
    else if (GLOBAL::mode == NORMALIZE) {
      GLOBAL::logfilename = GLOBAL::basefilename + "normalize" + log_suffix;
    }
    else {
      GLOBAL::logfilename = GLOBAL::basefilename + log_suffix;
    }

	tracemsg(10, "in main(): open log file", GLOBAL::logfilename.c_str());
    //GLOBAL::logfile.close();
    GLOBAL::logfile.open(GLOBAL::logfilename.c_str());
    if (!GLOBAL::logfile) {
      throw FileOpenError(GLOBAL::logfilename);
    }
    print_options(GLOBAL::logfile, argc, argv, env);

    if (GLOBAL::mode == MODELING) {
	  tracemsg(10, "in main(): enter mode ", "MODELING");	
      fzymodel(infilename1, infilename2);
    }
    else if (GLOBAL::mode == PRINT_SETS) {
      fzy2sets(infilename1, infilename2);
    }
    else if (GLOBAL::mode == ESTIMATION) {
      fzyestim(infilename1, infilename2);
    }
    else if (GLOBAL::mode == SIMULATION) {
      // /// estimation with order > 0
      fzyestim(infilename1, infilename2); 
    }
    else if (GLOBAL::mode == MAKE_DATA) {
      fzymkdat(infilename1, infilename2, infilename3);
    }
    else if (GLOBAL::mode == NORMALIZE) {
      fzynorml(infilename1, infilename2);
    }
    else {
      assert(1==0);
    }
  } // /// end toplevel try
   
  // /// toplevel catch
  catch(Error& error) {
    string msg = (string)"\n" + GLOBAL::prgname 
      + ": error: " + error.msg();
    exit_on_error(cerr, msg);
  }
  catch(char* errmsg) {
    string msg = (string)"\n" + GLOBAL::prgname 
      + ": error message caught in main(): " + errmsg;
    exit_on_error(cerr, msg);

  }
  catch(...) {
    string msg = (string)"\n" + GLOBAL::prgname
      + ": main(): fatal error: caught unexpected error type!!!";
    cerr << flush << msg << endl << flush;
    exit_on_error(cerr, msg);
  }
  
  // /// everything ok
  tracemsg(1, "in main(): everything ok, leaving trace file", GLOBAL::tracefilename.c_str());
  GLOBAL::logfile.close();
  GLOBAL::tracefile.close();
  if (GLOBAL::tracelevel < 1) {
	int return_value = remove(GLOBAL::tracefilename.c_str());
	if (return_value != 0) {
	  string msg = (string)"\n" + GLOBAL::prgname
        + ": main(): warning: could not delete trace file: " + GLOBAL::tracefilename;
      cerr << flush << msg << endl << flush;
	}
  }
  return 0;
}


// //////////////////////////////////////////////////////////////////////

void
exit_with_help(std::ostream& s) {
  if (GLOBAL::mode == MODELING) {
    s << "NAME\n"
      << "      " << GLOBAL::prgname << ": fully automated creation of"
      << " Sugeno type neuro-fuzzy models\n"
#ifdef TRAPEZOIDAL_FSETS
      << "        (trapezoidal fuzzy sets)"
#else
      << "        (sigmoidal fuzzy sets)"
#endif
      << " by deep learning based on input/output data.\n\n" 
      << "SYNOPSIS\n"
      << "      " << GLOBAL::prgname 
      << " -f1 <learn_data> -f2 <validation_data> [OPTIONS]\n\n"
      << "PARAMETERS\n"
      << "      -f1 <learn_data>       input file of learning patterns\n"
      << "      -f2 <validation_data>  input file of validation patterns\n\n"
      << "OPTIONS\n"
      << "      -c <cons_dim>          consequence dimension; default: "
      << GLOBAL::consequence_dimension << endl
      << "      -r <min_n_rules>       minimal number of rules; default: "
      << GLOBAL::min_n_rules << endl
      << "      -R <max_n_rules>       maximal number of rules; default: "
      << GLOBAL::max_n_rules << endl
      << "      -am                    equal adjacent mu; default: "
      << GLOBAL::adjacent_equal_mu << "\n"
      << "      -g [<alpha> [<beta>]]  use gradient descent instead of RPROP\n"
      << "                             (with learn rate and momentum)\n"
      << "      -ph <y_order>          fine tune (parallel) using HOOKE_JEEVES"
      << "\n"
      << "      -pr <y_order>          fine tune (parallel) using ROSENBROCK\n"
      << "      -pO <max_opt_iterat>   max opt iterations for parallel tuning;"
      << " default: " << GLOBAL::max_opt_iterations_parallel << endl
      << "      -P                     switch of premise parameter update;"
      << " default: " << GLOBAL::update_premise << " (1 = update)\n"
      << "      -C                     optimize consequence parameters with"
      << " SVD; default: " << GLOBAL::consequence_optimize_SVD << endl
      << "      -o  <min_opt_iterat>   min optimization iterations; default: "
      << GLOBAL::min_opt_iterations << endl
      << "      -O  <max_opt_iterat>   max optimization iterations; default: "
      << GLOBAL::max_opt_iterations << endl
      << "      -s  <steps_per_valid.> optim. steps per validation; default: "
      << GLOBAL::steps_per_validation << endl
      << "      -S  <shortcuts>        shortcuts when modeling; default: "
      << GLOBAL::shortcut << " (no shortcuts)\n"
      << "      -Fs <min_sigma>        minimal steepness of fuzzy sets;"
      << " default: " << GLOBAL::min_sigma << endl
      << "      -FS <max_sigma>        maximal steepness of fuzzy sets;"
      << " default: " << GLOBAL::max_sigma << endl
      << "      -i  <R2_improvement>   terminate if imp. gain < i; default: "
      << GLOBAL::R2_improvement << endl
      << "      -I  <best_R2_improvm.> take best if imp. gain > I; default: "
      << GLOBAL::R2_improvement << endl
      << "      -l  <exponent>         local consequence optimization;"
      << " default: " << GLOBAL::local_cons_optimization << endl
      << "      -L  <Lp-Norm>          use Lp-Norm (p = 1, 2 or 3); default: "
      << GLOBAL::norm << endl
      << "      -n                     denormalize; default: "
      << GLOBAL::denormalize << "\n"
      << "      -t  <trace_level>      trace level (for SW tests); default: "
      << GLOBAL::tracelevel << endl
      << "      -v  <verbose_level>    verbose level (for modelling); default: "
      << GLOBAL::verbose << endl
      << "      -e  <name_extension>   filename extension; default: `"
      << GLOBAL::filename_extension << "'\n"
      << "      -q                     quiet; no output on stdout and stderr\n"
      << "      -h                     print this help and exit\n\n";
  }
  else if (GLOBAL::mode == ESTIMATION) {
    s << "NAME\n"
      << "      " << GLOBAL::prgname << ": print estimation for data by"
      << " a given Sugeno type fuzzy model"
#ifdef TRAPEZOIDAL_FSETS
      << " (trapezoidal fuzzy sets).\n\n"
#else
      << " (sigmoidal fuzzy sets).\n\n"
#endif
      << "SYNOPSIS\n"
      << "      " << GLOBAL::prgname 
      << " -f1 <fuzzy_model> -f2 <data> [OPTIONS]\n\n"
      << "PARAMETERS\n"
      << "      -f1 <fuzzy_model>      input file of fuzzy model\n"
      << "      -f2 <data>             input file of data to estimate\n\n"
      << "OPTIONS\n"
      << "      -Dph <mu> <inc> <dec> <l> Page-Hinkley Detector on error with"
      << " mean mu,\n"
      << "                             increase, decrease, and sensitivity"
      << " lambda\n"
      << "      -n                     denormalize; default: "
      << GLOBAL::denormalize << "\n"
      << "      -v <verbose_level>     verbose level; default: "
      << GLOBAL::verbose << endl
      << "      -e <name_extension>    filename extension; default: `"
      << GLOBAL::filename_extension << "'\n"
      << "      -E <error_offset>      offset when printing RMS; default: `"
      << GLOBAL::error_offset << "'\n"
      << "      -q                     quiet; no output on stdout and stderr\n"
      << "      -h                     print this help and exit\n\n";
  }
  else if (GLOBAL::mode == SIMULATION) {
    s << "NAME\n"
      << "      " << GLOBAL::prgname << ": print simulation for data by"
      << " a given Sugeno type fuzzy model"
#ifdef TRAPEZOIDAL_FSETS
      << " (trapezoidal fuzzy sets).\n\n"
#else
      << " (sigmoidal fuzzy sets).\n\n"
#endif
      << "SYNOPSIS\n"
      << "      " << GLOBAL::prgname 
      << " -f1 <fuzzy_model> -f2 <data> -o <dynamic order> [OPTIONS]\n\n"
      << "PARAMETERS\n"
      << "      -f1 <fuzzy_model>      input file of fuzzy model\n"
      << "      -f2 <data>             input file of data to estimate\n"
      << "      -d <dynamic order>     order of the output's recurrency\n\n"
      << "OPTIONS\n"
      << "      -Dph <mu> <inc> <dec> <l> Page-Hinkley Detector on error with"
      << " mean mu,\n"
      << "                             increase, decrease, and sensitivity"
      << " lambda\n"
      << "      -n                     denormalize; default: "
      << GLOBAL::denormalize << "\n"
      << "      -v <verbose_level>     verbose level; default: "
      << GLOBAL::verbose << endl
      << "      -e <name_extension>    filename extension; default: `"
      << GLOBAL::filename_extension << "'\n"
      << "      -E <error_offset>      offset when printing RMS; default: `"
      << GLOBAL::error_offset << "'\n"
      << "      -q                     quiet; no output on stdout and stderr\n"
      << "      -h                     print this help and exit\n\n";
  }
  else if (GLOBAL::mode == PRINT_SETS) {
    s << "NAME\n"
      << "      " << GLOBAL::prgname << ": print fuzzy sets of a given"
      << " Sugeno type fuzzy model"
#ifdef TRAPEZOIDAL_FSETS
      << " (trapezoidal fuzzy sets).\n\n"
#else
      << " (sigmoidal fuzzy sets).\n\n"
#endif
      << "SYNOPSIS\n"
      << "      " << GLOBAL::prgname 
      << " -f1 <fuzzy_model> [OPTIONS]\n\n"
      << "PARAMETERS\n"
      << "      -f1 <fuzzy_model>      input file of fuzzy model\n\n"
      << "OPTIONS\n"
      << "      -s                     scale range of u; default: no\n"
      << "      -p <n_pixels>          number of pixels; default: 100\n"
      << "      -n                     denormalize; default: "
      << GLOBAL::denormalize << "\n"
      << "      -N                     print F(x)/(sum F(x)) instead of F(x);"
      << " default: " << GLOBAL::global_fset_value << "\n"
      << "      -v <verbose_level>     verbose level; default: "
      << GLOBAL::verbose << endl
      << "      -e <name_extension>    filename extension; default: `"
      << GLOBAL::filename_extension << "'\n"
      << "      -q                     quiet; no output on stdout and stderr\n"
      << "      -h                     print this help and exit\n\n";
  }
  else if (GLOBAL::mode == MAKE_DATA) {
    s << "NAME\n"
      << "      " << GLOBAL::prgname << ": create data for fuzzy"
      << " modeling"
#ifdef TRAPEZOIDAL_FSETS
      << " (trapezoidal fuzzy sets).\n\n"
#else
      << " (sigmoidal fuzzy sets).\n\n"
#endif
      << "SYNOPSIS\n"
      << "      " << GLOBAL::prgname 
      << " -f1 <norm_file> -f2 <input_data> -f3 <output_data>"
      << " -d <d1 d2 ... dn> [OPTIONS]\n\n"
      << "PARAMETERS\n"
      << "      -f1 <norm_file>        filename of normalization specification"
      << "\n"
      << "      -f2 <input_data>       filename of input (source) data\n"
      << "      -f3 <output_data>      filename for output\n"
      << "      -d  <int_vector>       data dynamics specification (for "
      << " x1, x2,...,y)\n\n"
      << "OPTIONS\n"
      << "      -Np <n_patterns>       no. of training patterns to make"
      << " (0 = all); default: "
      << GLOBAL::n_training_data << endl
      << "      -Ns <n_patterns>       no. of patterns to skip at the"
      << " beginning; default: "
      << GLOBAL::n_skip_data << endl
      << "      -rt                    regressor starts with u(t);"
      << " default: " << GLOBAL::ut_regressor << "\n"
      << "      -o                     do not print original data; default: "
      << GLOBAL::denormalize << "\n"
      << "      -v <verbose_level>     verbose level; default: "
      << GLOBAL::verbose << endl
      << "      -e <name_extension>    filename extension; default: `"
      << GLOBAL::filename_extension << "'\n"
      << "      -q                     quiet; no output on stdout and stderr\n"
      << "      -h                     print this help and exit\n\n";
  }
  else if (GLOBAL::mode == NORMALIZE) {
    s << "NAME\n"
      << "      " << GLOBAL::prgname << ": compute (linear) normalization"
      << " parameters.\n\n"
      << "SYNOPSIS\n"
      << "      " << GLOBAL::prgname 
      << " -f1 <input_data> -f2 <output_data> [OPTIONS]\n\n"
      << "PARAMETERS\n"
      << "      -f1 <input_data>       filename of input (source) data\n"
      << "      -f2 <norm_file>        filename for output\n"
      << "OPTIONS\n"
      << "      -sr                    scale by range;"
      << " default: " << GLOBAL::scale_by_range << "\n"
      << "      -sd <scale_factor>     scale by stddev (scale_factor * sigma);"
      << " default: " << GLOBAL::sigma_factor << "\n"
      << "      -u  <n_pattern>        use only n patterns (0 = all);"
      << " default: " << GLOBAL::scale_only_used_data << "\n"
      << "      -v <verbose_level>     verbose level; default: "
      << GLOBAL::verbose << endl
      << "      -e <name_extension>    filename extension; default: `"
      << GLOBAL::filename_extension << "'\n"
      << "      -q                     quiet; no output on stdout and stderr\n"
      << "      -h                     print this help and exit\n\n";
  }
  else { // GLOBAL::mode == UNDEFD_MODE
    s << GLOBAL::prgname << ": error: invalid mode!\n";
    assert(1 == 0);
  }
  s << "RELEASE\n"
    << "      software release " <<release<< " from " << release_date << ".\n"
    << "\n";
  s << "AUTHORS\n"
    << "      Manfred Maennle, with contribution svc.cc by Steffen Bloedt\n"
    << "      fzymodel@maennle.org\n"
    << "\n";
  s << "COPYRIGHT: MIT License\n"
    << " \n"
    << "  Copyright (c) 1999, 2020 Manfred Maennle\n"
    << " \n"
    << "  Permission is hereby granted, free of charge, to any person obtaining a copy\n"
    << "  of this software and associated documentation files (the \"Software\"), to deal\n"
    << "  in the Software without restriction, including without limitation the rights\n"
    << "  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell\n"
    << "  copies of the Software, and to permit persons to whom the Software is\n"
    << "  furnished to do so, subject to the following conditions:\n"
    << "  \n"
    << "  The above copyright notice and this permission notice shall be included in all\n"
    << "  copies or substantial portions of the Software.\n"
    << "  \n"
    << "  THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\n"
    << "  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\n"
    << "  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE\n"
    << "  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\n"
    << "  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\n"
    << "  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE\n"
    << "  SOFTWARE.\n";
  exit(0);
}
// //////////////////////////////////////////////////////////////////////

void
print_options(ofstream& strm, int argc, char* argv[], char* env[]) {
  tracemsg(100, "print_options() of main.cc: entering function", "print_options()");
  // /// print options and settings
  string msg = "options and settings:\n";
  msg += (string)"  command line:";
  for (int i=0; i<argc; ++i) {
    msg += (string)" " + argv[i];
  }
  tracemsg(100, "print_options() of main.cc: printing OK for loop", "argv");
  msg += (string)"\n";
  msg += (string)"  binary path: " + GLOBAL::binpath + "\n";
  msg += (string)"  program name: " + GLOBAL::prgname + "\n";
  msg += (string)"  software release: r" + release + "\n";
  msg += (string)"  variable type: " + REAL_NAME + "\n";
#ifdef TRAPEZOIDAL_FSETS
  msg += (string)"  shape of membership funtions: trapezoidal\n";
#else
  msg += (string)"  shape of membership funtions: sigmoidal\n";
#endif
  msg += (string)"  basic outfile name: \"" + GLOBAL::basefilename + "\"\n";
  msg += (string)"  name extension: \"" + GLOBAL::filename_extension + "\"\n";
  msg += (string)"  logfilename: " + GLOBAL::logfilename + "\n";
  msg += (string)"  verbose level: " + itos(GLOBAL::verbose) + "\n";
  msg += (string)"  trace level: " + itos(GLOBAL::tracelevel) + "\n";
  if (GLOBAL::tracelevel > 0) {
    msg += (string)"  tracefilename: " + GLOBAL::tracefilename + "\n";
  }
  msg += (string)"  take back normalization: "+itos(GLOBAL::denormalize)+"\n";
  tracemsg(100, "print_options() of main.cc: printing OK for loop", "general program info");
  if (GLOBAL::mode == MODELING) {
    msg += (string)"  mode: MODELING\n";
    if (GLOBAL::optimization == UNDEFD_ALGO) {
      msg += (string)"  optimization: UNDEFD (i.e. none)\n";
    }
    else if (GLOBAL::optimization == RPROP) {
      msg += (string)"  optimization: RPROP\n";
    }
    else if (GLOBAL::optimization == GRAD_DESCENT) {
      msg += (string)"  optimization: GRAD_DESCENT\n";
      msg += (string)"  learn rate alpha: " 
	+ dtos(GLOBAL::alpha) +"\n";    
      msg += (string)"  momentum beta: " 
	+ dtos(GLOBAL::beta) +"\n";    
    }
    else {
      msg += (string)"  optimization: " + itos(GLOBAL::optimization) + "\n";
    }
    if (GLOBAL::parallel_optimization == UNDEFD_ALGO) {
      msg += (string)"  parallel_optimization: UNDEFD (i.e. none)\n";
    }
    else if (GLOBAL::parallel_optimization == HOOKE_JEEVES) {
      msg += (string)"  parallel_optimization: HOOKE_JEEVES\n";
      msg += (string)"  order (of y): "	+ itos(GLOBAL::order) + "\n";
    }
    else if (GLOBAL::parallel_optimization == ROSENBROCK) {
      msg += (string)"  parallel_optimization: ROSENBROCK\n";
      msg += (string)"  order (of y): " + itos(GLOBAL::order) + "\n";
    }
    else {
      msg += (string)"  parallel_optimization: "
	+ itos(GLOBAL::parallel_optimization) + "\n";
    }
    msg += (string)"  adjacent fuzzy sets have equal mu: " 
      + itos(GLOBAL::adjacent_equal_mu) + "\n";    
    msg += (string)"  consequence dimension: " 
      + itos(GLOBAL::consequence_dimension) + "\n";
    msg += (string)"  local consequence optimization: "
      + itos(GLOBAL::local_cons_optimization) + "\n";
    msg += (string)"  norm of error vector: L"
      + itos(GLOBAL::norm) + "\n";
    msg += (string)"  shortcut: " 
      + itos(GLOBAL::shortcut) +"\n";
    msg += (string)"  max number of fuzzy rules: " 
      + itos(GLOBAL::max_n_rules) + "\n";
    msg += (string)"  min number of fuzzy rules: "
      + itos(GLOBAL::min_n_rules) + "\n";
    msg += (string)"  steps per validation: "
      + itos(GLOBAL::steps_per_validation) + "\n";
    msg += (string)"  max parallel optimization iterations: "
      + itos(GLOBAL::max_opt_iterations_parallel) + "\n";
    msg += (string)"  min optimization iterations: "
      + itos(GLOBAL::min_opt_iterations) + "\n";
    msg += (string)"  max optimization iterations: "
      + itos(GLOBAL::max_opt_iterations) + "\n";
    msg += (string)"  optimize premise parameters: "
      + itos(GLOBAL::update_premise) + "\n";
    msg += (string)"  optimize consequence with SVD: "
      + itos(GLOBAL::consequence_optimize_SVD) + "\n";
    msg += (string)"  minimal sigma: "
      + dtos(GLOBAL::min_sigma) + "\n";
    msg += (string)"  maximal sigma: "
      + dtos(GLOBAL::max_sigma) + "\n";
    msg += (string)"  R2 improvement: "
      + dtos(GLOBAL::R2_improvement) + "\n";
    msg += (string)"  best R2 improvement: "
      + dtos(GLOBAL::best_R2_improvement) + "\n";
    msg += (string)"  optimize epoch best: "
      + itos(GLOBAL::optimize_epoch_best) + "\n";
    msg += (string)"  optimize global best: "
      + itos(GLOBAL::optimize_global_best) + "\n";
  }
  else if (GLOBAL::mode == PRINT_SETS) {
    msg += (string)"  mode: PRINT_SETS\n";
    msg += (string)"  number of pixels: "
      + itos(GLOBAL::n_pixels) + "\n";
    msg += (string)"  print F(x)/(sum F(x)) instead of F(x): "
      + itos(GLOBAL::global_fset_value) + "\n";
    msg += (string)"  scale: "
      + itos(GLOBAL::scale) + "\n";
    msg += (string)"  tex precision: "
      + itos(GLOBAL::tex_precision) + "\n";
  }
  else if (GLOBAL::mode == SIMULATION) {
    msg += (string)"  mode: SIMULATION\n";
    msg += (string)"  order (of y): " + itos(GLOBAL::order) + "\n";
    msg += (string)"  error offset: " + dtos(GLOBAL::error_offset) + "\n";
  }
  else if (GLOBAL::mode == ESTIMATION) {
    msg += (string)"  mode: ESTIMATION\n";
    msg += (string)"  error offset: " + dtos(GLOBAL::error_offset) + "\n";
  }
  else if (GLOBAL::mode == MAKE_DATA) {
    msg += (string)"  mode: MAKE_DATA\n";
    msg += (string)"  no. of learning/training patterns: "
      + itos(GLOBAL::n_training_data) + "\n";
    msg += (string)"  no. of patterns to skip at the beginning: "
      + itos(GLOBAL::n_skip_data) + "\n";
    msg += (string)"  dynamic orders:";
    for (vector<int>::iterator porder = GLOBAL::dynamic_orders.begin();
	 porder != GLOBAL::dynamic_orders.end();
	 ++porder) {
      msg += (string)" " + itos(*porder);
    }
    msg += (string)"\n";
  }
  else if (GLOBAL::mode == NORMALIZE) {
    msg += (string)"  mode: NORMALIZE\n";
    msg += (string)"  scale only used data: "
      + itos(GLOBAL::scale_only_used_data) + "\n";
    msg += (string)"  scale by standard deviation: "
      + itos(GLOBAL::scale_by_standard_deviation) + "\n";
    msg += (string)"  scale by range: "
      + itos(GLOBAL::scale_by_range) + "\n";
    msg += (string)"  sigma factor: "
      + dtos(GLOBAL::sigma_factor) + "\n";
  }
  else {
	tracemsg(200, "print_options() of main.cc: start printing for mode", "UNDEFINED");
    msg += (string)"  mode: UNDEFINED\n";
  }
  if (GLOBAL::page_hinkley) {
    msg += (string)"  page_hinkley: "
      + itos(GLOBAL::page_hinkley) + "\n";
    msg += (string)"    mu_0: "
      + dtos(GLOBAL::page_hinkley_mu_0) + "\n";
    msg += (string)"    nu_inc: "
      + dtos(GLOBAL::page_hinkley_nu_inc) + "\n";
    msg += (string)"    nu_dec: "
      + dtos(GLOBAL::page_hinkley_nu_dec) + "\n";
    msg += (string)"    lambda: "
      + dtos(GLOBAL::page_hinkley_lambda) + "\n";
  }
  msg += (string)"\n";
  if ((!GLOBAL::quiet) && (GLOBAL::verbose > 1)) {
    cerr << msg;
  }
  if (GLOBAL::logfile) {
    GLOBAL::logfile << msg;
  }
  if (GLOBAL::tracefile) {
    GLOBAL::tracefile << msg << flush;
  }
  return;
}

// //////////////////////////////////////////////////////////////////////

void 
exit_on_msg(std::ostream& strm, const string& message) {
  string msg = (string)GLOBAL::prgname + ": error: " + message + "\n"
    + "type \"" + GLOBAL::prgname + " -h\" for help\n";
  cout << flush;
  cerr << flush;
  if (!GLOBAL::quiet) {
    strm << flush << msg;
  }
  if (!GLOBAL::logfile) {
    GLOBAL::logfilename = (string)GLOBAL::prgname + log_suffix;
    GLOBAL::logfile.open(GLOBAL::logfilename.c_str());
  }
  if (GLOBAL::logfile) {
    GLOBAL::logfile << msg;
  }
  if (GLOBAL::tracefile) {
    GLOBAL::tracefile << msg << flush;
  }
  exit(1);
}

// //////////////////////////////////////////////////////////////////////

void 
exit_on_error(std::ostream& strm, const string& msg) {
  cout << flush;
  cerr << flush;
  if (!GLOBAL::quiet) {
    strm << flush << msg << endl << flush;
  }
  if (!GLOBAL::logfile) {
    GLOBAL::logfilename = (string)GLOBAL::prgname + log_suffix;
    GLOBAL::logfile.open(GLOBAL::logfilename.c_str());
  }
  if (GLOBAL::logfile) {
    GLOBAL::logfile << flush << msg << endl << flush;
  }
  if (GLOBAL::tracefile) {
    GLOBAL::tracefile << msg << flush;
  }
  exit(-1);
}
