#ifndef FUNCT_HH
#define FUNCT_HH

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
 * $Id: funct.hh,v 2.9 2020-08-29 10:47:36+00:00:02 manfred Exp $
 */



#ifndef WIN2017
#include <vector.h>  // STL vectors
#else
#include <vector>    // STL vectors
#endif


#include "global.hh"
#include "param.hh"



class Function {
protected:
  const size_t dim_;
  Real value_;
  vector<Parameter*> parameters_;
public:
  Function() : dim_(0) {}
  Function(size_t d) : dim_(d) { parameters_.resize(d); }
  virtual ~Function() {}
  const size_t dim() const { return dim_; }
  const Real& value() const { return value_; }
  const vector<Parameter*>& parameters() const { return parameters_; }
  size_t dim() { return dim_; }
  vector<Parameter*>& parameters() { return parameters_; }
  virtual Real calculate() = 0;
  virtual Real validate() = 0;
  virtual void derivate() = 0;
};

std::ostream& operator << (std::ostream& strm, const Function& f);



// //////////////////////////////////////////////////////////////////////


class EllipsoidFunction : public Function {
protected:
  vector<SimpleParam> x_;
public:
  EllipsoidFunction() {}
  EllipsoidFunction(size_t d) : Function(d) { 
    x_.resize(d);
    vector<Parameter*>::iterator pp = parameters_.begin();
    vector<SimpleParam>::iterator px = x_.begin();
    while (px != x_.end()) {
      *(pp++) = &*(px++);
    } 
  }
  ~EllipsoidFunction() {}
  const vector<SimpleParam>& x() const { return x_; }
  vector<SimpleParam>& x() { return x_; }
  Real calculate();
  Real validate() { return calculate(); }
  void derivate();
};


/*************************************************
 *
 * Class: BananaFunction
 *
 * Added by Steffen Bloedt
 * Date: 31.01.2000
 *
 *************************************************
 */

class BananaFunction : public Function {
protected:
  vector<SimpleParam> x_;
public:
  BananaFunction() {}
  BananaFunction(size_t d) : Function(d) { 
    x_.resize(d); 
    vector<Parameter*>::iterator pp = parameters_.begin();
    vector<SimpleParam>::iterator px = x_.begin();
    while (px != x_.end()) {
      *(pp++) = &*(px++);
    } 
  }
  ~BananaFunction() {}
  const vector<SimpleParam>& x() const { return x_; }
  vector<SimpleParam>& x() { return x_; }
  Real calculate();
  Real validate() { return calculate(); }
  void derivate();
};

#endif  // #ifndef FUNCT_HH
