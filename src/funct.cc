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
 * $Id: funct.cc,v 2.9 2020-08-29 10:47:36+00:00:02 manfred Exp $
 */


#include <assert.h> 

#include "global.hh"
#include "param.hh"
#include "funct.hh"

std::ostream& operator << (std::ostream& strm, const Function& f) {
  strm << f.dim() << " " << f.value() << "  ";
  vector<Parameter*>::const_iterator p = f.parameters().begin();
  while (p != f.parameters().end()) {
    strm << **(p++) << " " ;
  }
  return strm;
}


// //////////////////////////////////////////////////////////////////////


Real EllipsoidFunction::calculate() {
  assert(parameters_.size() == dim_);
  const Real c = 1.0/9.0;
  Real temp1 = parameters_[0]->value() - parameters_[1]->value();
  Real temp2 = parameters_[0]->value() + parameters_[1]->value() - 10.0;
  value_ = temp1*temp1 + c*temp2*temp2;
  return value_;
}


void EllipsoidFunction::derivate() {
  assert(parameters_.size() == dim_);
  const Real c = 1.0/9.0;
  Parameter* p0 = parameters_[0];
  Parameter* p1 = parameters_[1];
  Real temp1 = p0->value() - p1->value();
  Real temp2 = p0->value() + p1->value() - 10.0;
  p0->grad_1() = p0->grad();
  p1->grad_1() = p1->grad();
  p0->grad() = 2.0 * (temp1 + c*temp2);
  p1->grad() = 2.0 * (-temp1 + c*temp2);
  return;
}


/*************************************************
 *
 * Class: BananaFunction
 *
 * Added by Steffen Bloedt
 * Date: 31.01.2000
 *
 *************************************************
 */

Real BananaFunction::calculate() {
  assert(parameters_.size() == dim_);
  Real x0 = parameters_[0]->value();
  Real x1 = parameters_[1]->value();
  value_ = (100*(x0*x0-x1)*(x0*x0-x1)+(1-x0)*(1-x0));
  return value_;
}


void BananaFunction::derivate() {
  assert(parameters_.size() == dim_);
  Parameter* p0 = parameters_[0];
  Parameter* p1 = parameters_[1];
  Real x0 = p0->value();
  Real x1 = p1->value();
  
  p0->grad_1() = p0->grad();
  p1->grad_1() = p1->grad();
  p0->grad() = 400*x0*(x0*x0-x1)-2*(1-x0);
  p1->grad() = -200*(x0*x0-x1);
  return;
}
