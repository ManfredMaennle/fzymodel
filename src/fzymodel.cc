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
 * $Id: fzymodel.cc,v 2.9 2020-08-29 10:47:38+00:00:02 manfred Exp $
 */


#ifndef WIN2017
#include <fstream.h>
#else
#include <fstream>
#endif

#include "global.hh"
#include "data.hh"
#include "fmodel.hh"
#include "minimize.hh"
#include "main.hh"

#include "fzymodel.hh"


void
fzymodel(char* learnfilename, char* validationfilename) throw (Error) {
  Data a;
  Data b;
  a.load(learnfilename);
  b.load(validationfilename); 
  assert(a.U().size() > 0);
  assert(b.U().size() > 0);
  assert(a.udim() == b.udim());
  assert(a.scale_factor() == b.scale_factor());
  assert(a.scale_shift() == b.scale_shift());
  assert(GLOBAL::consequence_dimension > 0);

  if (GLOBAL::consequence_dimension > 1 + a.udim()) {
    throw(ConsdimError(itos(GLOBAL::consequence_dimension), 
		       itos(1 + a.udim())));
  }
  assert(GLOBAL::consequence_dimension <= 1 + a.udim());

  size_t epoch = 0;
  size_t best_epoch = 0;
  FModel epoch_model(a, GLOBAL::consequence_dimension);
  epoch_model.learnfilename() = (string)learnfilename;
  epoch_model.validationfilename() = (string)validationfilename;
  epoch_model.learn_data() = &a;
  epoch_model.valid_data() = &b;
  FModel global_model;
  FModel best_model;
  Real epoch_error;
  //#WIN2017 Real global_error;
  Real best_error = FLT_MAX;
  Real epoch_R2;
  Real global_R2;
  Real best_R2 = 0.0;

  string r2filename = GLOBAL::basefilename + "_rr" + r2_suffix;
  string errfilename = GLOBAL::basefilename + "_rr" + error_suffix;
  string outfilename;
  string modfilename;
  ofstream modfile;
  ofstream r2file(r2filename.c_str());
  if (!r2file) {
    throw FileOpenError(r2filename);
  }
  ofstream errfile(errfilename.c_str());
  if (!errfile) {
    throw FileOpenError(errfilename);
  }

  // initial one-rule model; not optimized
  outfilename = GLOBAL::basefilename + "_r" + itos(epoch) + output_suffix;
  modfilename = GLOBAL::basefilename + "_r" + itos(epoch) + model_suffix;
  modfile.open(modfilename.c_str());
  if (!modfile) {
    throw FileOpenError(modfilename);
  }
  epoch_error = epoch_model.estimation(b, outfilename.c_str());
  epoch_R2 = epoch_model.R2(b);
  errfile << epoch_error << endl;
  r2file << epoch_R2 << endl;
  modfile << epoch_model; 
  modfile.close();

// ////////////////////////////////////////////////////////////////////

//   RPROPMinimizer rprop_minimizer;
//   HookeJeevesMinimizer hooke_jeeves_minimizer;
//   Minimizer* m;
//   m = &hooke_jeeves_minimizer;

//   Function* fff = &epoch_model;
//   PRINT("model.create_parameters");
//   epoch_model.create_parameters(&a, &b);
//   PRINT("fff->calculate");
//   fff->calculate();
//   PRINT("fff->validate");
//   fff->validate();
//   PRINT(epoch_model);

//   PRINT("m->minimize");
//   m->minimize(fff);
//   //  m->minimize(&epoch_model);
//   PRINT(epoch_model);
//   return;

// ////////////////////////////////////////////////////////////////////

  // initial one-rule model; consequences optimized using SVD
  ++epoch;
  outfilename = GLOBAL::basefilename + "_r" + itos(epoch) + output_suffix;
  modfilename = GLOBAL::basefilename + "_r" + itos(epoch) + model_suffix;
  modfile.open(modfilename.c_str());
  if (!modfile) {
    throw FileOpenError(modfilename);
  }
  epoch_model.optimize_SVD(a);
  epoch_error = epoch_model.estimation(b, outfilename.c_str());
  epoch_R2 = epoch_model.R2(b);
  errfile << epoch_error << endl;
  r2file << epoch_R2 << endl;
  modfile << epoch_model; 
  modfile.close();
  best_model.copy(epoch_model);
  best_error = epoch_error;
  best_R2 = epoch_R2;
  best_epoch = epoch;
  // /// print currently best model
  {
    string bestmodfilename = GLOBAL::basefilename + "_ro" + model_suffix;
    ofstream bestmodfile(bestmodfilename.c_str());
    if (!bestmodfile) {
      throw FileOpenError(bestmodfilename);
    }
    bestmodfile << "### only currently best model!\n";
    bestmodfile << "### (i.e. uncaught exception later on)\n";
    bestmodfile << best_model;
    bestmodfile.close();
  }
  verbose(0, "#################### epoch", epoch);
  verbose(0, "cdim", GLOBAL::consequence_dimension);
  verbose(0, "epoch_model", epoch_model);
  verbose(0, "epoch_error", epoch_error);
  verbose(0, "epoch_R2", epoch_R2);

  // /// Sugeno's heuristic search
  do { // next epoch
    global_model.copy(epoch_model);
    //#WIN2017 global_error = epoch_error;
    global_R2 = epoch_R2;
    ++epoch;
    verbose(0, "#################### epoch", epoch);
    outfilename = GLOBAL::basefilename + "_r" + itos(epoch) + output_suffix;
    modfilename = GLOBAL::basefilename + "_r" + itos(epoch) + model_suffix;
    modfile.open(modfilename.c_str());
    if (!modfile) {
      throw FileOpenError(modfilename);
    }
    epoch_error = REAL_MAX;
    // for all rules
    size_t first_rule = 0;
    size_t last_rule = epoch-1;
    if ((GLOBAL::shortcut % 2) == 1) {
      first_rule = global_model.worst_rule_index(a);
      last_rule = first_rule + 1;
    }
    for(size_t rule = first_rule; rule < last_rule; ++rule) { 
      // for all variables
      for(size_t variable = 0; variable < a.udim(); ++variable) { 
	FModel candidate_model(global_model, rule, variable);
	Real candidate_error = REAL_MAX;
	if (GLOBAL::reset_cons) {
	  candidate_model.reset_consequences();
	}
	if (GLOBAL::optimization == RPROP) {
	  candidate_model.RPROP_init();
	  candidate_error = 
	    candidate_model.optimize_RPROP(a, b, 
					   GLOBAL::min_opt_iterations,
					   GLOBAL::max_opt_iterations);
	}
	else if (GLOBAL::optimization == GRAD_DESCENT) {
	  candidate_model.GRAD_DESCENT_init();
	  candidate_error = 
	    candidate_model.optimize_GRAD_DESCENT(a, b, 
						  GLOBAL::min_opt_iterations,
						  GLOBAL::max_opt_iterations);
	}
	else {
	  throw Error((string)"fzymodel(): invalid optimization algorithm!");
	}
	candidate_error = candidate_model.estimation(b);
	if (GLOBAL::verbose > 1) {
	  string output=(string)"cdim = "+itos(GLOBAL::consequence_dimension);
	  output += (string)", epoch = " + itos(epoch);
	  output += (string)", rule = " + itos(rule);
	  output += (string)", variable = " + itos(variable);
	  output += (string)", candidate error = " + dtos(candidate_error);
	  output += "\n";
	  if (! GLOBAL::quiet) {
	    cout << output; 
	  }
	  if (GLOBAL::logfile) {
	    GLOBAL::logfile << output;
	  }
	}
	if (candidate_error < epoch_error) {
	  epoch_model.copy(candidate_model);
	  epoch_error = candidate_error;
	}
      } // end for all variables
    } // end for all rules
    // additional training of best epoch model
    if (GLOBAL::optimize_epoch_best > 0) {
	if (GLOBAL::optimization == RPROP) {
	  epoch_error =  
	    epoch_model.optimize_RPROP(a, b, 1, 
				       GLOBAL::optimize_epoch_best);
	}
	else if (GLOBAL::optimization == GRAD_DESCENT) {
	  epoch_error =  
	    epoch_model.optimize_GRAD_DESCENT(a, b, 1, 
					      GLOBAL::optimize_epoch_best);
	}
	else {
	  throw Error((string)"fzymodel(): invalid optimization algorithm!");
	}
    }
    // print best epoch model and its estimation 
    epoch_error = epoch_model.estimation(b, outfilename.c_str());
    epoch_R2 = epoch_model.R2(b);
    r2file << epoch_R2 << endl;
    errfile << epoch_error << endl;
    modfile << epoch_model;
    modfile.close();
    verbose(0, "epoch_model", epoch_model);
    verbose(0, "epoch_error", epoch_error);
    verbose(0, "epoch_R2", epoch_R2);
    //    if (epoch_R2 > best_R2 + GLOBAL::best_R2_improvement) {
    if (epoch_error < best_error) {
      best_model.copy(epoch_model);
      best_error = epoch_error;
      best_R2 = epoch_R2;
      best_epoch = epoch;
      // /// print currently best model
      string bestmodfilename = GLOBAL::basefilename + "_ro" + model_suffix;
      ofstream bestmodfile(bestmodfilename.c_str());
      if (!bestmodfile) {
	throw FileOpenError(bestmodfilename);
      }
      bestmodfile << "### only currently best model!\n";
      bestmodfile << "### (i.e. uncaught exception later on)\n";
      bestmodfile << best_model;
      bestmodfile.close();
    }
  } while( ((epoch_R2 > global_R2 + GLOBAL::R2_improvement)
	    && (epoch < GLOBAL::max_n_rules))
	   || (epoch < GLOBAL::min_n_rules) );
  // /// end of Sugeno's heuristic search
  r2file.close();
  errfile.close();

  best_error = best_model.estimation(a);
  verbose(1, "estimation RMS for training data A", best_error);
  if (GLOBAL::order > 0) {
    best_error = best_model.simulation(a);
    verbose(1, "simulation RMS for training data A", best_error);
  }
  best_error = best_model.estimation(b);
  verbose(1, "estimation RMS for validation data B", best_error);
  if (GLOBAL::order > 0) {
    best_error = best_model.simulation(b);
    verbose(1, "simulation RMS for validation data B", best_error);
  }

  // /// additional series-parallel training of best global model
  if (GLOBAL::optimize_global_best > 0) {
    string msg = "additional series-parallel optimization of best model";
    verbose(0, "message", msg);
    epoch_model.copy(best_model);
    epoch_error = best_error;
    if (GLOBAL::optimization == RPROP) {
      epoch_error = 
	epoch_model.optimize_RPROP(a, b, 1, GLOBAL::optimize_global_best);
    }
    else if (GLOBAL::optimization == GRAD_DESCENT) {
      epoch_error = 
	epoch_model.optimize_GRAD_DESCENT(a, b,1,GLOBAL::optimize_global_best);
    }
    else {
      throw Error((string)"fzymodel(): invalid optimization algorithm!");
    }
    epoch_error = epoch_model.estimation(b);
    if (epoch_error < best_error) {
      best_model.copy(epoch_model);
      best_error = epoch_error;
    }
    best_error = best_model.estimation(a);
    verbose(1, "estimation RMS for training data A", best_error);
    if (GLOBAL::order > 0) {
      best_error = best_model.simulation(a);
      verbose(1, "simulation RMS for training data A", best_error);
    }
    best_error = best_model.estimation(b);
    verbose(1, "estimation RMS for validation data B", best_error);
    if (GLOBAL::order > 0) {
      best_error = best_model.simulation(b);
      verbose(1, "simulation RMS for validation data B", best_error);
    }
    // best_model.optimize_SVD(a); // /// do final consequence optimization
  }

  // /// additional parallel training of best global model
  if (GLOBAL::parallel_optimization != UNDEFD_ALGO) {
    RPROPMinimizer rprop_minimizer;
    HookeJeevesMinimizer hooke_jeeves_minimizer;
    Minimizer* minimizer;
    if (GLOBAL::parallel_optimization == HOOKE_JEEVES) {
      minimizer = &hooke_jeeves_minimizer;
    }
    else if (GLOBAL::parallel_optimization == ROSENBROCK) {
      throw Error((string)"fzymodel(): ROSENBROCK not yet available!");
    }
    else {
      throw Error((string)"fzymodel(): invalid par. optimization algorithm!");
    }
    //   Function* f = &best_model;
    //   f->optimize_SVD(a);
    //   f->RPROP_init();
    //   f->optimize_RPROP(a, b, GLOBAL::min_opt_iterations,
    // 		    GLOBAL::max_opt_iterations);
    //   PRINT(*f);
    // PRINT(best_model);

    string msg = "additional parallel optimization of best model";
    verbose(0, "message", msg);
    best_model.create_parameters(&a, &b);
    minimizer->minimize(&best_model);
    best_model.erase_parameters();

    best_error = best_model.estimation(a);
    verbose(1, "estmation RMS for training data A", best_error);
    if (GLOBAL::order > 0) {
      best_error = best_model.simulation(a);
      verbose(1, "simulation RMS for training data A", best_error);
    }
    best_error = best_model.estimation(b);
    verbose(1, "estmation RMS for validation data B", best_error);
    if (GLOBAL::order > 0) {
      best_error = best_model.simulation(b);
      verbose(1, "simulation RMS for validation data B", best_error);
    }
  } // /// end if (GLOBAL::parallel_optimization != UNDEFD_ALGO) 

  // /// print overall best model
  best_error = best_model.estimation(b);
  best_R2 = best_model.R2(b); 
  string bestr2filename = GLOBAL::basefilename + "_ro" + r2_suffix;
  string besterrfilename = GLOBAL::basefilename + "_ro" + error_suffix;
  string bestoutfilename = GLOBAL::basefilename + "_ro" + output_suffix;
  string bestmodfilename = GLOBAL::basefilename + "_ro" + model_suffix;
  ofstream bestr2file(bestr2filename.c_str());
  if (!bestr2file) {
    throw FileOpenError(bestr2filename);
  }
  ofstream besterrfile(besterrfilename.c_str());
  if (!besterrfile) {
    throw FileOpenError(besterrfilename);
  }
  ofstream bestmodfile(bestmodfilename.c_str());
  if (!bestmodfile) {
    throw FileOpenError(bestmodfilename);
  }
  ofstream bestoutfile(bestoutfilename.c_str());
  if (!bestoutfile) {
    throw FileOpenError(bestoutfilename);
  }
  verbose(0, "#################### best epoch", best_epoch);
  verbose(0, "cdim", GLOBAL::consequence_dimension);
  verbose(0, "best_model", best_model);
  verbose(0, "best_error", best_error);
  verbose(0, "best_R2", best_R2);
  bestr2file << "### validation data: \"" << validationfilename << "\"\n";
  bestr2file << "### best epoch: " << best_epoch << endl;
  bestr2file << best_R2 << endl;
  best_error = best_model.estimation(b, bestoutfilename.c_str());
  besterrfile << "### validation data: \"" << validationfilename << "\"\n";
  besterrfile << "### best epoch: " << best_epoch << endl;
  besterrfile << best_error << endl;
  bestmodfile << "### globally best model\n";
  bestmodfile << best_model;
  bestr2file.close();
  besterrfile.close();
  bestmodfile.close();
  bestoutfile.close();
  return;
}
