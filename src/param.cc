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
 * $Id: param.cc,v 2.9 2020-08-29 10:47:40+00:00:02 manfred Exp $
 */


#include "global.hh"
#include "param.hh"


Real dummy_value2 = 0.0;


std::ostream& operator << (std::ostream &strm, const Parameter& par) {
  strm << par.dim() << " " << par.value() << " " << par.delta() << " " 
       << par.grad() << " " << par.grad_1() << " ";
  return strm;
}


std::ostream& operator << (std::ostream &strm, const vector<Parameter*>& v)
{
  if (v.begin() == v.end()) {
    strm << "[empty vector<Parameter*>] ";
    return strm;
  }
  vector<Parameter*>::const_iterator p;
  for (p=v.begin(); p!=v.end(); ++p) {
    strm << **p << endl;
  }
  return strm;
}
