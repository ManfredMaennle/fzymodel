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
 * $Id: minimize.cc,v 2.9 2020-08-29 10:47:39+00:00:02 manfred Exp $
 */


#ifndef WIN2017
#include <vector.h>  // STL vectors
#include <assert.h>
#else
#include <vector>    // STL vectors
#include <assert.h>
#endif


#include "global.hh"
#include "param.hh"
#include "funct.hh"

#include "minimize.hh"



void RPROPMinimizer::update(Function* f) {
  vector<Parameter*>::iterator pp = f->parameters().begin();
  while (pp != f->parameters().end()) {
    Parameter* p = *(pp++);
    if (p->grad() * p->grad_1() >= 0.0) {
      if (p->grad() * p->grad_1() > 0.0) {
	p->delta() *= eta_plus_;
      }
      if (p->grad() > 0.0) {
	p->add_value( - p->delta());
      }
      else if (p->grad() < 0.0) {
	p->add_value(p->delta());
      }
    }
    else { // if (p->grad() * p->grad_1() < 0.0) 
      if (p->grad_1() > 0.0) {
	p->add_value(p->delta());
      }
      else if (p->grad_1() < 0.0) {
	p->add_value( - p->delta());
      }
      p->grad() = 0.0;
      p->grad_1() = 0.0;
      p->delta() *= eta_minus_;
    }
    p->check_bounds();
  }
  return;
}


void RPROPMinimizer::backstep(Function* f) {
  vector<Parameter*>::iterator pp = f->parameters().begin();
  while (pp != f->parameters().end()) {
    Parameter* p = *(pp++);
    if (p->grad_1() > 0.0) {
      p->add_value(p->delta());
    }
    else if (p->grad_1() < 0.0) {
      p->add_value( - p->delta());
    }
  }
  return;
}


void RPROPMinimizer::minimize(Function* f) {
  // / cross validate after each N_OPTS_PER_STEP iterations of RPROP
  size_t iteration = 0;
  //#WIN2017 Real calc_fitness = REAL_MAX;
  //#WIN2017 Real valid_fitness = REAL_MAX;
  Real sum_calc_fitness = REAL_MAX;  
  Real sum_valid_fitness = REAL_MAX;
  while (iteration < max_opt_iterations_) {
    try {
      Real new_calc_fitness = 0.0;
      Real new_valid_fitness = 0.0;
      Real new_sum_calc_fitness = 0.0;
      Real new_sum_valid_fitness = 0.0;
      size_t k = 0;
      while (k < steps_per_validation_) {
	f->derivate();
	update(f);
	// hold out method (cross validation)
	new_calc_fitness = f->calculate();
	new_valid_fitness = f->validate();
	new_sum_calc_fitness += new_calc_fitness;
	new_sum_valid_fitness += new_valid_fitness; 
	++iteration;
	++k;
      }
      // new_valid_fitness = f->validate();
      if (iteration < min_opt_iterations_)  {
	// do further optimize (in any case)
      }
      else { // check for success
	if ( (new_sum_calc_fitness < sum_calc_fitness) 
	     && (new_sum_valid_fitness < sum_valid_fitness) ) {
	  // success: do further optimize
	  //#WIN2017 calc_fitness = new_calc_fitness;
	  //#WIN2017 valid_fitness = new_valid_fitness;
	  sum_calc_fitness = new_sum_calc_fitness;
	  sum_valid_fitness = new_sum_valid_fitness;
	}
	else {
	  // /// take back last update step and stop optimization
	  backstep(f);
	  --iteration;
	  break;
	}
      }
    } // end try
    catch(FunctionUndefinedError& error) {
      string msg = (string)"warning from RPROPMinimizer::minimize(): ";
      msg += error.msg();
      cerr << msg << endl;
      backstep(f);
      --iteration;
      break;
    } // end catch
  } // end while (iteration < max_iterations) 
  return;
}


/*************************************************
 *
 * Class: HookeJeevesMinimizer
 *
 * Added by Steffen Bloedt
 * Date: 31.01.2000
 *
 *************************************************
 */

const Real HookeJeevesMinimizer::REDU = 0.5;
const Real HookeJeevesMinimizer::INIT_DELTA = 5E-2;
const Real HookeJeevesMinimizer::MIN_DELTA = REAL_EPSILON;
const Real HookeJeevesMinimizer::MIN_IMPROVEMENT = REAL_EPSILON;
//const Real HookeJeevesMinimizer::MIN_IMPROVEMENT = REAL_MIN;
//const Real HookeJeevesMinimizer::MIN_IMPROVEMENT = 0.0;

void HookeJeevesMinimizer::minimize(Function* f)
{

  // Initialize some variables:
  delta = INIT_DELTA;
  iteration = 0;

  // Check dimensionality
  size_t dim = f->parameters().size();
  assert(dim > 0);
  if (dim == 0) {
    return;
  }

  // Create some vectors:
  t = vector<Real>(dim);
  x = vector<Real>(dim);

  // Initialize the vectors with the appropriate values:
  for (size_t i = 0; i < dim; i++) {
    f->parameters()[i]->check_bounds();
    x[i] = f->parameters()[i]->value();
  }

  bool successful_explore = false;
  y = f->calculate();
  Real yd = y;
  
  while ( (delta >= MIN_DELTA)
	  && (iteration < GLOBAL::max_opt_iterations_parallel)
	  ) {
    /* The (iterations < maxiterations) term is repeated here in order
     * to avoid problems with termination. We also need to check the
     * difference between the current y-value and the new yd-value. If
     * this difference is too small, we terminate. With that we avoid
     * problems with a too small t-vector.
     */
    while ( (iteration < GLOBAL::max_opt_iterations_parallel)
	    && explore(f, y, yd, t)
	    && ((fabs(y - yd)) >= MIN_IMPROVEMENT)
	    ) {
      y = yd;
      for (size_t i = 0; i < dim; i++) {
	//t[i] = f->parameters()[i]->value() - x[i];
	x[i] = f->parameters()[i]->value();
	f->parameters()[i]->add_value(t[i]);
	f->parameters()[i]->check_bounds();
      }
      yd = f->calculate();
      iteration++;
      successful_explore = true;
    }
    
    if (successful_explore) {
      for (size_t i = 0; i < dim; i++) {
	//f->parameters()[i]->value() -= t[i];
	f->parameters()[i]->set_value(x[i]);
      }
      yd = f->calculate();
      iteration++;
      successful_explore = false;
    }
    else {
      delta = delta * REDU;
      verbose(1, "current Hooke/Jeeves opt. iteration", iteration);
      verbose(1, "new search step (delta)", delta);
      verbose(1, "current error", y);
    }
  }
  
  // Set the best parameters:
  for (size_t i = 0; i < dim; i++) {
    f->parameters()[i]->set_value(x[i]);
  }

  verbose(0, "total no. of Hooke/Jeeves optimization iterations", iteration);
  return;
}




bool HookeJeevesMinimizer::explore(Function* f, Real y, Real& yd,
				   vector<Real>& t) {
  Real yq = 0;
  /* This variable determines if the value of xd was really changed.
   * This is necessary if you want to make Hooke/Jeeves accept not only
   * parameters where the new error is less than the old error, but also
   * parameters where both errors are equal.
   */
  bool xdChanged = false;
  
  for (size_t i = 0; i < f->parameters().size(); ++i) {
    Real oldvalue = f->parameters()[i]->value();

    // search in positive direction
    f->parameters()[i]->set_value(oldvalue + delta);
    f->parameters()[i]->check_bounds();
    yq = f->calculate();
    iteration++;    
    //if (yq <= yd) {
    if (yq < yd) {
      yd = yq;
      xdChanged = true;
      t[i] = f->parameters()[i]->value() - oldvalue;
      continue;
    }

    // search in negative direction
    f->parameters()[i]->set_value(oldvalue - delta);
    f->parameters()[i]->check_bounds();
    yq = f->calculate();
    iteration++;
    //if (yq <= yd) {
    if (yq < yd) {
      yd = yq;
      xdChanged = true;
      t[i] = f->parameters()[i]->value() - oldvalue;
      continue;
    }

    // no improvement possible
    f->parameters()[i]->set_value(oldvalue);

  } // end for(...)

  Real t_length = 0.0;
  for (vector<Real>::const_iterator i = t.begin(); i != t.end(); ++i) {
    //t_length += *i * *i;
    t_length += fabs(*i);
  }
  //t_length = sqrt(t_length);
  
  //  return ((yd <= y) && xdChanged && (t_length > 10 * REAL_EPSILON));
  return ((yd < y) && xdChanged && (t_length > REAL_EPSILON));
  //return ((yd < y) && xdChanged);
}



// /*************************************************
//  *
//  * Class: RosenbrockMinimizer
//  *
//  * Added by Steffen Bloedt
//  * Date: 31.01.2000
//  *
//  *************************************************
//  */




// const size_t RosenbrockMinimizer::MAX_ITERATION = 150000;


// const Real RosenbrockMinimizer::THRESHOLD = 1E-6;


// const bool RosenbrockMinimizer::THRESHOLD_TYPE = false;


// const Real RosenbrockMinimizer::L_PLUS = 3;


// const Real RosenbrockMinimizer::L_MINUS = -0.5;


// const Real RosenbrockMinimizer::DELTA_MAX = 1E+15;;


// const Real RosenbrockMinimizer::DELTA_MIN = 1E-70;;


// const Real RosenbrockMinimizer::INIT_DELTA = 5E-2;



// RosenbrockMinimizer::RosenbrockMinimizer() : Minimizer()
// {
// }


// void RosenbrockMinimizer::minimize(Function* f)
// {
//   (*this).f = f;
//   initialize();
  
//   Vector xq = Vector(dim);
//   Real yQ;
//   y = f->calculate();
//   iteration++;
//   Vector deltasum = Vector(dim);
//   /* This is needed to make sure that each parameter was optimized
//    * unsuccessful after a susseccful step before the base vectors will
//    * be changed.
//    */
//   vector<int> successArray = vector<int>(dim);
//   size_t failure, j;
  
//   try {
//     do {
//       // Initialize the delta, deltasum and success vectors:
//       for (size_t i = 0; i < dim; i++) {
// 	delta.setValue(i, INIT_DELTA);
// 	deltasum.setValue(i, 0);
// 	successArray[i] = -1;
//       }
//       failure = 0;
      
//       do {
// 	j = 0;
// 	do {
// 	  xq = x.add(base[j].multiply(delta.getValue(j)));
	  
// 	  // Set the parameters:
// 	  for (size_t i = 0; i < dim; i++) {
// 	    f->parameters()[i]->value() = xq.getValue(i);
// 	  }
	  
// 	  yQ = f->calculate();
// 	  iteration++;
// 	  if (yQ < y) {
// 	    //if (yQ <= y) {
// 	    deltasum.setValue(j, deltasum.getValue(j) + delta.getValue(j));
// 	    if (fabs(delta.getValue(j)) < DELTA_MAX) {
// 	      delta.setValue(j, L_PLUS * delta.getValue(j));
// 	    }
// 	    x = xq;
// 	    y = yQ;
// 	    if (successArray[j] == -1) {
// 	      successArray[j] = 0;
// 	    }
// 	  }
// 	  else {
// 	    if (fabs(delta.getValue(j)) > DELTA_MIN) {
// 	      delta.setValue(j, L_MINUS * delta.getValue(j));
// 	    }
// 	    if (successArray[j] == 0) {
// 	      failure++;
// 	      successArray[j] = 1;
// 	    }
// 	  }
// 	  j++;
// 	} while (j < dim);
//       } while (failure < dim && (THRESHOLD_TYPE?y:delta.euklidNorm()) > THRESHOLD &&
// 	       iteration < MAX_ITERATION);
//       try {
// 	gramSchmidt(base, deltasum);
//       }
//       catch (IndexException) {
// 	/* Abort the optimization, because a base-vector is the 0-vector
// 	 * which cannot be normalized:
// 	 */
// 	break;
//       }
//     } while ((THRESHOLD_TYPE?y:delta.euklidNorm()) > THRESHOLD && iteration < MAX_ITERATION);
//   } catch (IndexException) {
//     cout << "IndexException: An error occured during the";
//     cout << " RosenbrockMinimizer minimization (minimize())." << endl;
//     exit(-1);
//   }
//   /* Reassign the values of x to the model, because it is possible,
//    * that xq was last assigned to the model.
//    */
//   for (size_t i = 0; i < dim; i++) {
//     f->parameters()[i]->value() = xq.getValue(i);
//   }
// }


// void RosenbrockMinimizer::initialize()
// {
//   // Initialize some variables:
//   iteration = 0;
  
//   // Create some vectors:
//   f->parameters() = f->parameters();
//   dim = f->parameters().size();
//   delta = Vector(dim);
//   base.resize((int) dim);
//   x = Vector(dim);
  
//   // Initialize the vectors with the appropriate values:
//   for (size_t i = 0; i < dim; i++) {
//     f->parameters()[i]->check_bounds();
//     x.setValue(i, f->parameters()[i]->value());
//     // Create the basis vectors:
//     base[i] = Vector(dim);
//     for (size_t j = 0; j < dim; j++) {
//       base[i].setValue(j, (i == j)?1:0);
//     }
//   }
// }


// void RosenbrockMinimizer::gramSchmidt(vector<Vector>& base, Vector& deltasum)
// throw (IndexException)
// {
//   vector<Vector> wBase;
//   // Initialize wBase.
//   wBase.resize(dim);
//   for (size_t i = 0; i < dim; i++) {
//     wBase[i] = Vector(dim);
//     for (size_t j = 0; j < dim; j++) {
//       wBase[i].setValue(j, 0);
//     }
//   }
  
//   // Generate the starting vectors of the new base.
//   for (size_t i = 0; i < dim; i++) {
//     for (size_t j = i; j < dim; j++) {
//       wBase[i] = wBase[i].add(base[j].multiply(deltasum.getValue(j)));
//     }
//   }
//   // The first new base vector
//   base[0] = wBase[0].normalize();
  
//   // Generate the other new base vectors with the method of Gram-Schmidt.
//   for (size_t i = 1; i < dim; i++) {
//     Vector helpVec = Vector(dim);
//     for (size_t j = 0; j < i; j++) {
//       helpVec = helpVec.add(base[j].multiply(wBase[i].multiply(base[j])));
//     }
//     wBase[i] = wBase[i].sub(helpVec);
//     base[i] = wBase[i].normalize();
//   }
// }


