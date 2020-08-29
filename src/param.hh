#ifndef PARAM_HH
#define PARAM_HH

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
 * $Id: param.hh,v 2.9 2020-08-29 10:47:40+00:00:02 manfred Exp $
 */


/* includes:
 */

#ifndef WIN2017
#include <vector.h>  // STL vectors
#else
#include <vector>    // STL vectors
#endif


#include "global.hh"


class Parameter {
protected:
  size_t dim_;
  Real* value_;
  Real* delta_;
  Real* grad_;
  Real* grad_1_;
public:
  Parameter() {}
  Parameter(size_t dim, Real* v, Real* d, Real* g, Real* g1) : dim_(dim),
    value_(v), delta_(d), grad_(g), grad_1_(g1) {}
  const size_t& dim() const { return dim_; }
  const Real& value() const { return *value_; }
  const Real& delta() const { return *delta_; }
  const Real& grad() const { return *grad_; }
  const Real& grad_1() const { return *grad_1_; }
  size_t& dim() { return dim_; }
  Real& delta() { return *delta_; }
  Real& grad() { return *grad_; }
  Real& grad_1() { return *grad_1_; }
  virtual ~Parameter() {}
  virtual void set_value(Real x) { *value_ = x; }
  virtual void add_value(Real x) { *value_ += x; }
  virtual void check_bounds() = 0;
  friend std::ostream& operator << (std::ostream &strm, const Parameter& par);
};


std::ostream& operator << (std::ostream &strm, const vector<Parameter*>& v);


class SimpleParam : public Parameter {
public:
  SimpleParam() {}
  SimpleParam(size_t dim, Real* value, Real* delta, Real* g, Real* g1) 
    : Parameter(dim, value, delta, g, g1) {}
  ~SimpleParam() {}
  void check_bounds() {}
};


class ConsParam : public Parameter {
protected:
public:
  ConsParam() {}
  ConsParam(size_t dim, Real* v, Real* d, Real* g, Real* g1) 
    : Parameter(dim, v, d, g, g1) {}
  ConsParam(const ConsParam& par) : Parameter(par) {}
  ~ConsParam() {}
  void check_bounds() {}
//   void check_bounds(){
//     if (*delta_ > max_delta_cons) {
//       *delta_ = max_delta_cons;
//     }
//     else if (*delta_ < min_delta_cons) {
//       *delta_ = min_delta_cons;
//     }
//   }
};


extern Real dummy_value2;

class MuParam : public Parameter {
protected:
  Real* value2_;
public:
  MuParam() {}
  MuParam(size_t dim, Real* v, Real* d, Real* g, Real* g1) 
    : Parameter(dim, v, d, g, g1) {}
  ~MuParam() {}
  void init_value2(Real* p) { value2_ = p; }
  void set_value(Real x) { *value_ = x; *value2_ = x; }
  void add_value(Real x) { *value_ += x; *value2_ += x; }
  void check_bounds() {}
//   void check_bounds(){
//     if (*delta_ > max_delta_mu) {
//       *delta_ = max_delta_mu;
//     }
//     else if (*delta_ < min_delta_mu) {
//       *delta_ = min_delta_mu;
//     }
//   }
};


class SigmaParam : public Parameter {
protected:
public:
  SigmaParam() {}
  SigmaParam(size_t dim, Real* v, Real* d, Real* g, Real* g1) 
    : Parameter(dim, v, d, g, g1) {}
  ~SigmaParam() {}
  void add_value(Real x) { 
    Real new_val = *value_ + x;
    if (*value_ < 0.0) {
      if ((new_val < -GLOBAL::min_sigma) && (new_val > -GLOBAL::max_sigma)) {
	*value_ = new_val;
      }
    }
    else {
      if ((new_val > GLOBAL::min_sigma) && (new_val < GLOBAL::max_sigma)) {
	*value_ = new_val;
      }
    }
  }
  void check_bounds() {}
//     if (*delta_ > max_delta_sigma) {
//       *delta_ = max_delta_sigma;
//     }
//     else if (*delta_ < min_delta_sigma) {
//       *delta_ = min_delta_sigma;
//     }
//     if (*value_ > max_sigma) {
//       *value_ = max_sigma;
//     }
//     else if (*value_ < min_sigma) {
//       *value_ = min_sigma;
//     }
//  }
};


#endif /// #ifndef PARAM_HH
