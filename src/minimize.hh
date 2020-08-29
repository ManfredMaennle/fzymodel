#ifndef MINIMIZE_HH
#define MINIMIZE_HH

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
 * $Id: minimize.hh,v 2.9 2020-08-29 10:47:39+00:00:02 manfred Exp $
 */


#ifndef WIN2017
#include <vector.h>  // STL vectors
// #include <vec.h>     // TNT vectors
#else
#include <vector>    // STL vectors
// #include <tnt.h>     // TNT vectors
#endif


#include "global.hh"
#include "param.hh"
#include "funct.hh"


class Minimizer {
public:
  Minimizer() {}
  virtual ~Minimizer() {}
  virtual void minimize(Function* f) = 0;
};


class RPROPMinimizer : public Minimizer {
public:
  Real eta_plus_;
  Real eta_minus_;
  size_t steps_per_validation_;
  size_t min_opt_iterations_;
  size_t max_opt_iterations_;
protected:
  void update(Function* f);
  void backstep(Function* f);
public:
  RPROPMinimizer() : eta_plus_(1.2), eta_minus_(0.5), steps_per_validation_(4),
    min_opt_iterations_(12), max_opt_iterations_(500) {}
  ~RPROPMinimizer() {}
  // uses f->calculate(), f->derivate(), f->validate(), f->parameters()
  void minimize(Function* f);
};


/*************************************************
 *
 * Class: HookeJeevesMinimizer
 *
 * Added by Steffen Bloedt
 * Date: 31.01.2000
 *
 *************************************************
 */

class HookeJeevesMinimizer : public Minimizer {
  private:
  /** The maximal number of iterations.
   */
  static const size_t MAX_ITERATION;
  /** Improvement threshold. The optimization stops when the improvement
   * is less to this threshold.
   */
  static const Real MIN_IMPROVEMENT;
  /** Factor for reduction of step size.
   */
  static const Real REDU;
  /** The initial delta-value.
   */
  static const Real INIT_DELTA;
  /** The minimal delta-value; terminate if delta < MIN_DELTA.
   */
  static const Real MIN_DELTA;
  
  /** Vector with the currently best parameters.
   */
  vector<Real> x;
  /** The current function value.
   */
  Real y;
  /** The current step size.
   */
  Real delta;
  /** The current number of iterations.
   */
  size_t iteration;
  /** The difference vector.
   */
  vector<Real> t;
  
  
  public:
  /** Constructor.
   */
  HookeJeevesMinimizer() {}
  
   
  /** Start minimization.
   *
   * @param f  The function which will be minimized.
   */
  virtual void minimize(Function* f);
  
  
  /** Destructor. This desctructor is needed, because there are virtual
   * functions in this class.
   */
  ~HookeJeevesMinimizer() {}
  
  
 private:
  
  /** Explore the surroundings of xd.
   *
   * @param y  The current function-value.
   * @param yd  The new function-value.
   *
   * @return True, if the exploration was successful; false when not.
   */
  bool explore(Function* f, Real y, Real& yd, vector<Real>& t);

};


// /*************************************************
//  *
//  * Class: RosenbrockMinimizer
//  *
//  * Added by Steffen Bloedt
//  * Date: 31.01.2000
//  *
//  *************************************************
//  */

// #include "Vector.h"

// class RosenbrockMinimizer : public Minimizer
// {
//  private:
//   /** The maximal number of iterations.
//    */
//   static const size_t MAX_ITERATION;
//   /** Threshold. The optimization stops when the step size
//    * is less or equal to this threshold.
//    */
//   static const Real THRESHOLD;
//   /** The type of the threshold.
//    * If you set this to "true", the threshold determines a lower bound
//    * of the function values. If the function value gets below this
//    * bound, the algorithm terminates. In this case, THRESHOLD can be
//    * any real number.
//    * If you set this to "false", the threshold determines a lower
//    * bound of the Euklid-norm of the delta-vector. If the Euklid-Norm
//    * of the delta-vector gets below this bound, the algorithm terminates.
//    * In this case, THRESHOLD has to be always greater or equal to 0.
//    */
//   static const bool THRESHOLD_TYPE;
//   /** Factor to increase the step-size. (lambda+ = 3)
//    */
//   static const Real L_PLUS;
//   /** Factor to decrease the step-size. (lambda- = -0.5)
//    */
//   static const Real L_MINUS;
//   /** The max. delta value.
//    */
//   static const Real DELTA_MAX;
//   /** The min. delta value.
//    */
//   static const Real DELTA_MIN;
//   /** The initial delta-value.
//    */
//   static const Real INIT_DELTA;
  
//   /** The function that will be minimized.
//    */
//   Function* f;
//   /** Vector with pointers to all parameters.
//    */
//   vector<Parameter*> parameterPs;
//   /** The dimension of the parameter space to optimize.
//    */
//   size_t dim;
//   /** Vector with all parameters.
//    */
//   Vector x;
//   /** The base vectors.
//    */
//   vector<Vector> base;
//   /** Step size.
//    */
//   Vector delta;
//   /** The current function value.
//    */
//   Real y;
//   /** The current number of iterations.
//    */
//   size_t iteration;
  
  
//   public:
//   /** Constructor.
//    */
//   RosenbrockMinimizer();
  
  
//   /** Start minimization.
//    *
//    * @param f  The function which will be minimized.
//    */
//   virtual void minimize(Function* f);
  
  
//   /** Destructor. This desctructor is needed, because there are virtual
//    * functions in this class.
//    */
//   virtual ~RosenbrockMinimizer() {};

  
//   private:
//   /** Initialize optimization.
//    */
//   virtual void initialize();

  
//   /** Create a new basis with the method of Gram-Schmidt.
//    *
//    * @param base  The old base-vectors.
//    * @param deltasum  The summed up delta values.
//    */
//   void gramSchmidt(vector<Vector>& base, Vector& deltasum) throw (IndexException);  
  
// };


#endif /// #ifndef MINIMIZE_HH
