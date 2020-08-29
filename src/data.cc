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
 * $Id: data.cc,v 2.9 2020-08-29 10:47:35+00:00:02 manfred Exp $
 */



#ifndef WIN2017
#include <fstream.h>
#include <list.h>
#else
#include <fstream>
#include <list>
#endif


#include "global.hh"
#include "data.hh"

#include "data_lex.h"
#undef yyFlexLexer
#define yyFlexLexer data_FlexLexer
#include <FlexLexer.h>


std::ostream& operator<<(std::ostream &strm, const vector<Real> &v) {
  vector<Real>::const_iterator p = v.begin();
  while (p != v.end()) {
    strm << *(p++) << " ";
  }
  return strm;
}

std::ostream& operator<<(std::ostream &strm, const Uvector &v) {
  size_t N=v.dim();
  for (size_t i=0; i<N; i++)
    strm << v[i] << " ";
  // strm << endl;
  return strm;
}


std::ostream& operator << (std::ostream& strm, const Data& d) {
  strm << "### filename: \"" << d.filename_ << "\"\n";
  vector<Uvector>::const_iterator pU = d.U_.begin();
  vector<Real>::const_iterator py = d.y_.begin();
  size_t blockline = 0;
  while (pU != d.U_.end()) {
    strm << *(pU++);
    strm << *(py++) << endl;
    if (++blockline == d.blocksize()) {
      strm << endl;
      blockline = 0;
    }
  }
  return strm;
}


Data::Data(Subscript N, size_t M, Real x0) 
{
  filename_ = "";
  blocksize_ = 0;
  mean_y_ = 0.0;
  variance_y_ = 0.0;
  U_.resize(M);
  vector<Uvector>::iterator pU = U_.begin();  
  while (pU != U_.end()) {
    pU->newsize(N);
    Uvector::iterator p = pU->begin();  
    while (p != pU->end()) {
      *(p++) = x0;
    }
    ++pU;
  }
  y_.resize(M);
  vector<Real>::iterator py = y_.begin();
  while (py != y_.end()) {
    *(py++) = x0;
  }
  scale_factor_.resize(N+1, 1.0);
  scale_shift_.resize(N+1, 0.0);
}


void
Data::load(char* filename) throw (Error)
{
  filename_ = (string)filename;
  if (filename == NULL) {
    return;
  }
  ifstream file(filename);
  if (file == NULL) {
    throw FileOpenError(filename);
  }

  int n_columns = 0;
  int n_rows = 0;
  int column = 0;
  list<Real> patternvalues;
  list<Real> shiftvalues;
  list<Real> factorvalues;
  data_FlexLexer* lexer = new data_FlexLexer(&file);
  int token;
  size_t blockline = 0;
  int equal_blocksizes = 1;
  enum states { UNDEFD_STATE, PATTERNLINE, SHIFTLINE, FACTORLINE };
  states state = PATTERNLINE;

  blocksize_ = 0;
  mean_y_ = 0.0;
  variance_y_ = 0.0;

  do {
    token=lexer->yylex();
    switch (token) {
    case T_DATA_COMMENT:
      break;
    case T_DATA_NUMBER:
      if (state == PATTERNLINE) {
	patternvalues.push_back(atof(lexer->YYText()));
	++column;
      }
      else if (state == SHIFTLINE) {
	shiftvalues.push_back(atof(lexer->YYText()));
      }
      else if (state == FACTORLINE) {
	factorvalues.push_back(atof(lexer->YYText()));
      }
      else { 
	assert(1==0); // invalid state
      }
      break;
    case T_DATA_NEWLINE:
      if ((state == PATTERNLINE) && (column > 0)) { 
	/// / i.e., one row scanned (containing numbers)
	if (n_rows == 0) {
	  /// first row
	  n_columns = column;
	}
	else {
	  /// n_rows > 0
	  if (column != n_columns) {
	    string msg = "parse error in input file `" + (string)filename \
	      + "': line " + itos(lexer->lineno()) \
	      + ": number of columns not equal in each row!";
	    throw Error(msg);
	  }
	}
	++blockline;
	++n_rows;
	column = 0;
      }
      else if (state == PATTERNLINE) {
	/// / i.e., empty line scanned
	if (equal_blocksizes) {
	  if (blocksize_ == 0) { // / set initial blocksize
	    blocksize_ = blockline;
	  }
	  else { // / check for equal blocksize
	    if ( (blockline > 0) && (blocksize_ != blockline) ) {
	      equal_blocksizes = 0; 
	      blocksize_ = 0;
	    }
	  }
	  blockline = 0;
	}
      }
      else if (state == SHIFTLINE) {
	state = PATTERNLINE;
      }
      else if (state == FACTORLINE) {
	state = PATTERNLINE;
      }
      else {
	assert(1==0);
      }
      break;
    case T_DATA_SCALE_SHIFT:
      state = SHIFTLINE;
      break;
    case T_DATA_SCALE_FACTOR:
      state = FACTORLINE;
      break;
    case T_DATA_ERROR: 
      {
	string msg = "parse error in input file `" + (string)filename \
	  + "': line " + itos(lexer->lineno()) + ": lex error at `" \
	  + (string)lexer->YYText() + "'.";
	throw Error(msg);
      }
      break;
    case T_DATA_EOF:
      break;
    default:
      {
	string msg = "parse error: unknown token no.: " + itos(token);
	throw Error(msg);
	token = T_DATA_ERROR;
      }
    } /// end switch(token)
  } while((token != T_DATA_EOF) && (token != T_DATA_ERROR));
  file.close();
  if (n_columns < 1) {
    string msg = "error in input file `" + (string)filename \
      + "': n_columns < 1!";
    throw Error(msg);
  }
  // /// save patternvalues in data matrix
  U_.resize(n_rows);
  y_.resize(n_rows);
  list<Real>::const_iterator pl = patternvalues.begin();
  vector<Uvector>::iterator pU = U_.begin();  
  vector<Real>::iterator py = y_.begin();
  while (pU != U_.end()) {
    pU->newsize(n_columns-1);
    Uvector::iterator p = pU->begin();  
    while (p != pU->end()) {
      *(p++) = *(pl++);
    }
    ++pU;
    *(py++) = *(pl++);
  }
  // save shift and factor values in data object
  if ((shiftvalues.size() > 0) || (factorvalues.size() > 0)) {
    if (shiftvalues.size() != (unsigned int)n_columns) {
      string msg = "error in input file `" + (string)filename \
	+ "': no. of shift values != n_columns!";
      throw Error(msg);
    }
    if (factorvalues.size() != (unsigned int)n_columns) {
      string msg = "error in input file `" + (string)filename \
	+ "': no. of factor values != n_columns!";
      throw Error(msg);
    }
    scale_factor_.resize(n_columns);
    scale_shift_.resize(n_columns);
    copy(factorvalues.begin(), factorvalues.end(), scale_factor_.begin());
    copy(shiftvalues.begin(), shiftvalues.end(), scale_shift_.begin());
  }
  else {
    scale_factor_.resize(n_columns, 1.0);
    scale_shift_.resize(n_columns, 0.0);
  }
  
  // /// compute mean_y_ and variance_y_
  if (n_rows > 0) {
    py = y_.begin();
    while (py != y_.end()) {
      mean_y_ += *(py++);
    }
    mean_y_ /= (Real)n_rows;
    py = y_.begin();
    while (py != y_.end()) {
      Real difference = *(py++) - mean_y_;
      variance_y_ += difference * difference;
    }
    variance_y_ /= (Real)n_rows;
  }
  return;
}




void 
Data::normalize(const vector<Real>& factor, const vector<Real>& shift) 
  throw (Error) {
  size_t n_rows = y_.size();
  if (n_rows < 1) {
    return;
  }
  size_t n_columns = U_[0].size();
  if (n_columns+1 != (size_t)factor.size()) {
    string msg = "different n_columns in function";
    msg += " Data::normalize(...) for factor vector!";
    throw Error(msg);
  }
  if (n_columns+1 != (size_t)shift.size()) {
    string msg = "different n_columns in function";
    msg += " Data::normalize(...) for shift vector!";
    throw Error(msg);
  }
  // /// copy scale_factor_, scale_shift_
  vector<Real>::iterator pfactor = scale_factor_.begin();
  vector<Real>::iterator pshift = scale_shift_.begin();
  vector<Real>::const_iterator pf = factor.begin();
  vector<Real>::const_iterator ps = shift.begin();
  while (pfactor  != scale_factor_.end()) {
    *(pfactor++) = *(pf++); 
    *(pshift++) = *(ps++);
  }
  // /// normalize
  vector<Uvector>::iterator pU = U_.begin();  
  vector<Real>::iterator py = y_.begin();  
  while (py != y_.end()) {
    pfactor = scale_factor_.begin();
    pshift = scale_shift_.begin();
    Uvector::iterator p = pU->begin();  
    while (p != pU->end()) {
      *p = normalization(*p, *pfactor, *pshift);
      pshift++;
      pfactor++;
      p++;
    }
    *py = normalization(*py, *pfactor, *pshift);
    pU++;
    py++;
  }  
  // /// compute new mean_y_ and variance_y_
  if (n_rows > 0) {
    py = y_.begin();
    while (py != y_.end()) {
      mean_y_ += *(py++);
    }
    mean_y_ /= (Real)n_rows;
    py = y_.begin();
    while (py != y_.end()) {
      Real difference = *(py++) - mean_y_;
      variance_y_ += difference * difference;
    }
    variance_y_ /= (Real)n_rows;
  }
  return;
}
