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
 * $Id: fmodel.cc,v 2.9 2020-08-29 10:47:36+00:00:02 manfred Exp $
 */


#ifndef WIN2017
#include <fstream.h>
#include <sys/time.h>
#include <math.h>
#else
#include <fstream>
#include <sys/time.h>
#endif

#include "svd.hh"
#include "data.hh"
#include "param.hh"
#include "fzy_lex.h"

#include "fmodel.hh"


// /// offset when printing the error (y - y_hat) in the last column
// /// of the outfile
const Real err_offset = 0.0;


std::ostream& operator << (std::ostream& strm, const FSet& fset) 
{
  strm << fset.mu_ << " " << fset.sigma_ << " ";
  strm << fset.d_mu_ << " " << fset.d_sigma_ << " ";
  strm << fset.d_old_mu_ << " " << fset.d_old_sigma_ << " ";
  strm << fset.delta_mu_ << " " << fset.delta_sigma_ << " ";
  return strm;
}


std::ostream& operator << (std::ostream &strm, const FSetContainer& v)
{
  FSetContainer::const_iterator p;
  for (p=v.begin(); p!=v.end(); ++p) {
    strm << "  F" << 1 + (p - v.begin()) << ": " << *p << endl;
  }
  return strm;
}


std::ostream& operator << (std::ostream &strm, const vector<size_t>& v)
{
  vector<size_t>::const_iterator p;
  for (p=v.begin(); p!=v.end(); ++p) {
    strm << *p << ' ';
  }
  return strm;
}


/*
 * **********************************************************************
 * ********** FRule
 * **********************************************************************
 */

FRule::FRule(size_t u_dimension, size_t cons_dimension) 
{
  prem_.resize(2 * u_dimension);
  cons_.newsize(cons_dimension);
  d_cons_.newsize(cons_dimension);
  d_old_cons_.newsize(cons_dimension);
  delta_cons_.newsize(cons_dimension);
  Consequence::iterator pcons = cons_.begin();
  Consequence::iterator pd_cons = d_cons_.begin();
  Consequence::iterator pd_old_cons = d_old_cons_.begin();
  Consequence::iterator pdelta_cons = delta_cons_.begin();
  while (pcons != cons_.end()) {
    *(pcons++) = cons_0;
    *(pd_cons++) = 0.0;
    *(pd_old_cons++) = 0.0;
    *(pdelta_cons++) = delta_cons_0;
  }
}

// //////////////////////////////////////////////////////////////////////

void 
FRule::copy(const FRule& r, FSet* fset_begin, const FSet* r_fset_begin)
{
  if (prem_.size() != r.prem_.size()) {
    prem_.resize(r.prem_.size());
  }
  if (cons_.size() != r.cons_.size()) {
    cons_.newsize(r.cons_.size());
    d_cons_.newsize(r.cons_.size());
    d_old_cons_.newsize(r.cons_.size());
    delta_cons_.newsize(r.cons_.size());
  }
  Premise::iterator pprem1 = prem_.begin();
  Premise::const_iterator pprem2 = r.prem_.begin();
  while (pprem1 != prem_.end()) {
    if (*pprem2 == NULL) {
      *(pprem1++) = *(pprem2++);
    }
    else {
      *(pprem1++) = fset_begin + (*(pprem2++) - r_fset_begin);
    }
  }
  Consequence::iterator pcons1 = cons_.begin();
  Consequence::iterator pd_cons1 = d_cons_.begin();
  Consequence::iterator pd_old_cons1 = d_old_cons_.begin();
  Consequence::iterator pdelta_cons1 = delta_cons_.begin();
  Consequence::const_iterator pcons2 = r.cons_.begin();
  Consequence::const_iterator pd_cons2 = r.d_cons_.begin();
  Consequence::const_iterator pd_old_cons2 = r.d_old_cons_.begin();
  Consequence::const_iterator pdelta_cons2 = r.delta_cons_.begin();
  while (pcons1 != cons_.end()) {
    *(pcons1++) = *(pcons2++);
    *(pd_cons1++) = *(pd_cons2++);
    *(pd_old_cons1++) = *(pd_old_cons2++);
    *(pdelta_cons1++) = *(pdelta_cons2++);
  }
  return;
}

// //////////////////////////////////////////////////////////////////////

void 
FRule::refine(FRule* r, size_t u, FSet* newleftfset, FSet* newrightfset)
{
  /// left fset: sigma < 0
  /// right fset: sigma > 0
  //#WIN2017 FSet** thisoldright = (prem_.begin() + 2*u);
  //#WIN2017 FSet** thisoldleft = (prem_.begin() + 2*u+1);
  //#WIN2017 FSet** roldright = (r->prem_.begin() + 2*u);
  //FSet** thisoldright = &*(prem_.begin() + 2*u);
  //FSet** thisoldleft = &*(prem_.begin() + 2*u+1);
  //FSet** roldright = &*(r->prem_.begin() + 2*u);
  Premise::iterator thisoldright = prem_.begin() + 2*u;
  Premise::iterator thisoldleft = prem_.begin() + 2*u+1;
  Premise::iterator roldright = r->prem_.begin() + 2*u;
  Real mu_middle;
  Real diff_mu;
  /// set initial mu to the middle of the rule's region
  if (*thisoldright != NULL) {
      if (*thisoldleft != NULL) {
        /// right!=NULL, left!=NULL
	diff_mu = (*thisoldright)->mu() - (*thisoldleft)->mu();
        mu_middle = ((*thisoldright)->mu() + (*thisoldleft)->mu()) * 0.5;
      }
      else {
        /// right!=NULL, left==NULL
	diff_mu = (*thisoldright)->mu() - mu_left;
        mu_middle = ((*thisoldright)->mu() + mu_left) * 0.5;
      }
  }
  else {
    if (*thisoldleft != NULL) {
      /// right==NULL, left!=NULL
      diff_mu = mu_right - (*thisoldleft)->mu();
      mu_middle = (mu_right + (*thisoldleft)->mu()) * 0.5;
    }
    else {
      /// right==NULL, left==NULL
      diff_mu = (mu_right - mu_left);
      mu_middle = (mu_right + mu_left) * 0.5;
    }
  }
  /// limit diff_mu, e.g., don't set sigma too big (<= 4 * sigma_0)
//   if (diff_mu > 0.0) {
//     if (diff_mu < (mu_right-mu_left)/4.0) {
//       diff_mu = (mu_right-mu_left)/4.0;
//     }
//   }
//   else if (diff_mu < 0.0) {
//     if (diff_mu > -(mu_right-mu_left)/4.0) {
//       diff_mu = -(mu_right-mu_left)/4.0;
//     }
//   }
//   else { // if (diff_mu == 0.0) 
//     diff_mu = (mu_right-mu_left) * 1e-5;
//   }
  const Real max_sigma0_factor = 4.0;
  Real sigma_init = sigma_0 / diff_mu;
  if (sigma_init > 0.0) {
    if (sigma_init > max_sigma0_factor * sigma_0) {
      sigma_init = max_sigma0_factor * sigma_0;
    }
    if (sigma_init > GLOBAL::max_sigma) {
      sigma_init = GLOBAL::max_sigma;
    }
    if (sigma_init < GLOBAL::min_sigma) {
      sigma_init = GLOBAL::min_sigma;
    }
  }
  else {
    if (sigma_init < -max_sigma0_factor * sigma_0) {
      sigma_init = -max_sigma0_factor * sigma_0;
    }
    if (sigma_init < -GLOBAL::max_sigma) {
      sigma_init = -GLOBAL::max_sigma;
    }
    if (sigma_init > -GLOBAL::min_sigma) {
      sigma_init = -GLOBAL::min_sigma;
    }
  }
  newleftfset->set_mu(mu_middle);
//  newleftfset->set_sigma(-sigma_0 / diff_mu);
  newleftfset->set_sigma(-sigma_init);
  newrightfset->set_mu(mu_middle);
//  newrightfset->set_sigma(sigma_0 / diff_mu);
  newrightfset->set_sigma(sigma_init);
  *thisoldleft = newleftfset;
  *roldright = newrightfset;
  return;
}


/*
 * **********************************************************************
 * ********** FModel
 * **********************************************************************
 */

void 
FModel::copy(const FModel& oldmodel) 
{
  rdim_ = oldmodel.rdim_;
  udim_ = oldmodel.udim_;
  cdim_ = oldmodel.cdim_; 
  sdim_ = oldmodel.sdim_;
  // copy history;
  history_ = oldmodel.history_;
  // copy names
  learnfilename_ = oldmodel.learnfilename_;
  validationfilename_ = oldmodel.validationfilename_;
  // copy data pointers
  learn_data_ = oldmodel.learn_data_;
  valid_data_ = oldmodel.valid_data_;
  // copy old fsets
  fsets_.resize(sdim_);
  FSetContainer::iterator fsetnew = fsets_.begin();
  FSetContainer::const_iterator fsetold = oldmodel.fsets_.begin();
  while (fsetold != oldmodel.fsets_.end()) {
    *(fsetnew++) = *(fsetold++);
  }
  // copy old rules
  frules_.resize(rdim_);
  FRuleContainer::iterator frulenew = frules_.begin();
  FRuleContainer::const_iterator fruleold = oldmodel.frules_.begin();
  while (fruleold != oldmodel.frules_.end()) {
    //#WIN2017 (frulenew++)->copy(*(fruleold++), fsets_.begin(), oldmodel.fsets_.begin());
    (frulenew++)->copy(*(fruleold++), &*(fsets_.begin()), &*(oldmodel.fsets_.begin()));
  }
  return;
}

// //////////////////////////////////////////////////////////////////////

FModel::FModel(const FModel& oldmodel, size_t r, size_t u)
  : rdim_(oldmodel.rdim_ + 1), udim_(oldmodel.udim_), cdim_(oldmodel.cdim_), 
    sdim_(2 * (rdim_-1)) {
  assert(r >= 0);
  assert(r < oldmodel.rdim_);
  assert(u >= 0);
  assert(u < oldmodel.udim_);
  // copy names
  learnfilename_ = oldmodel.learnfilename_;
  validationfilename_ = oldmodel.validationfilename_;
  // copy data pointers
  learn_data_ = oldmodel.learn_data_;
  valid_data_ = oldmodel.valid_data_;
  // copy old fsets and add two new fsets;
  //#WIN2017 FSet* left;
  //#WIN2017 FSet* right;
  FSetContainer::iterator left;
  FSetContainer::iterator right;
  fsets_.resize(sdim_);
  FSetContainer::iterator fsetnew = fsets_.begin();
  FSetContainer::const_iterator fsetold = oldmodel.fsets_.begin();
  while (fsetold != oldmodel.fsets_.end()) {
    *(fsetnew++) = *(fsetold++);
  }
  left = fsetnew++;
  right = fsetnew;
  // copy old history and add new division
  history_.resize(sdim_);
  vector<size_t>::iterator historynew = history_.begin();
  vector<size_t>::const_iterator historyold = oldmodel.history_.begin();
  while (historyold != oldmodel.history_.end()) {
    *(historynew++) = *(historyold++);
  }
  *(historynew++) = r;
  *(historynew) = u;
  // copy rules and refine rule r in uindex u;
  frules_.resize(rdim_);
  FRuleContainer::iterator frulenew = frules_.begin();
  FRuleContainer::const_iterator fruleold = oldmodel.frules_.begin();
  size_t k=0; 
  while (k < rdim_) {
    if (k != r) {
      // copy old rule;
      //#WIN2017 (frulenew++)->copy(*(fruleold++),fsets_.begin(),oldmodel.fsets_.begin());
      (frulenew++)->copy(*(fruleold++), &*(fsets_.begin()), &*(oldmodel.fsets_.begin()));
      ++k;
    }
    else {
      // refine old rule and add new rule
      //#WIN2017 (frulenew++)->copy(*(fruleold),fsets_.begin(),oldmodel.fsets_.begin());
      //#WIN2017 (frulenew++)->copy(*(fruleold++),fsets_.begin(),oldmodel.fsets_.begin());
      //#WIN2017 (frulenew-2)->refine((frulenew-1), u, left, right);
      (frulenew++)->copy(*(fruleold), &*(fsets_.begin()), &*(oldmodel.fsets_.begin()));
      (frulenew++)->copy(*(fruleold++), &*(fsets_.begin()), &*(oldmodel.fsets_.begin()));
      (frulenew-2)->refine(&*(frulenew-1), u, &*left, &*right);
      k += 2;
    }
  }
}

// //////////////////////////////////////////////////////////////////////

void 
FModel::create_parameters(Data* learning_data, Data* validation_data) {
  learn_data_ = learning_data;
  valid_data_ = validation_data;
  if (GLOBAL::adjacent_equal_mu) {
    parameters_.reserve(rdim_*cdim_ + sdim_ + sdim_/2);
    mu_.reserve(sdim_ / 2);
  }
  else {
    parameters_.reserve(rdim_*cdim_ + 2*sdim_);
    mu_.reserve(sdim_);
  }
  cons_.reserve(rdim_ * cdim_);
  sigma_.reserve(sdim_);
  FRuleContainer::const_iterator r = frules_.begin();
  while (r != frules_.end()) {
    Consequence::iterator pcons = r->cons().begin();
    Consequence::iterator pd_cons = r->d_cons().begin();
    Consequence::iterator pd_old_cons = r->d_old_cons().begin();
    Consequence::iterator pdelta_cons = r->delta_cons().begin();
    for (size_t k=0; k<cdim_; ++k) {
      ConsParam c(k, pcons++, pdelta_cons++, pd_cons++, pd_old_cons++);
      cons_.push_back(c);
      //#WIN2017 parameters_.push_back(cons_.end() - 1);
      parameters_.push_back(&*(cons_.end() - 1));
    }
    ++r;
  }
  FSetContainer::iterator f = fsets_.begin();
  vector<size_t>::const_iterator h = history_.begin() + 1;
  int save_mu = 1;
  while (f != fsets_.end()) {
    MuParam m(*h, &(f->mu()), &(f->delta_mu()),
	      &(f->d_mu()), &(f->d_old_mu()));
    SigmaParam s(*h, &(f->sigma()), &(f->delta_sigma()),
		 &(f->d_sigma()), &(f->d_old_sigma()));
    if (GLOBAL::adjacent_equal_mu) {
      if (save_mu) {
	m.init_value2(&((f+1)->mu()));
	mu_.push_back(m);
	//#WIN2017 parameters_.push_back(mu_.end() - 1);
        parameters_.push_back(&*(mu_.end() - 1));
	// /// omit next mu
	save_mu = 0;
      }
      else {
	save_mu = 1;
      }
    }
    else {
      m.init_value2(&dummy_value2);
      mu_.push_back(m);
      //#WIN2017 parameters_.push_back(mu_.end() - 1);
      parameters_.push_back(&*(mu_.end() - 1));
    }
    sigma_.push_back(s);
    //#WIN2017 parameters_.push_back(sigma_.end() - 1);
    parameters_.push_back(&*(sigma_.end() - 1));
    h += 2;
    ++f;
  }
  return;
}


// //////////////////////////////////////////////////////////////////////


void 
FModel::erase_parameters() {
  parameters_.erase(parameters_.begin(), parameters_.end());
  cons_.erase(cons_.begin(), cons_.end());
  mu_.erase(mu_.begin(), mu_.end());
  sigma_.erase(sigma_.begin(), sigma_.end());
  return;
}



// //////////////////////////////////////////////////////////////////////

void 
FModel::load(char* filename) throw (Error) {
#ifdef YYDEBUG
  fzy_debug = 1;
#endif
  if (filename == NULL) {
    return;
  }
  else if (strcmp(filename, "stdin") == 0) {
    fzy_in = stdin;
  }
  else {
    fzy_in = fopen(filename, "r");
    if (fzy_in == NULL) {
      throw FileOpenError(filename);
    }
  }
  try {
    fzy_parse((void *)this);
  }
  catch(ParseError& error) {
    throw Error((string)"in file `" + filename + "' line " + error.msg());
  }
  if (strcmp(filename, "stdin") != 0) {
    fclose(fzy_in);
  }
  return;
}

// //////////////////////////////////////////////////////////////////////

Real 
FModel::y_hat(const Uvector& u) throw (Error) {
  assert(frules_.size() > 0);
  FRuleContainer::const_iterator pfrule = frules_.begin();
  register Real sum_premvalues = 0.0;
  register Real sum_rulevalues = 0.0;
  while (pfrule != frules_.end()) {
    register Real premvalue = pfrule->premvalue(u);
    sum_premvalues += premvalue;
    sum_rulevalues += inference(premvalue, (pfrule++)->consvalue(u));
  }
  if (sum_premvalues <= 0.0) {
    throw IncompleteCoverageError("FModel::y_hat()");
  }
  assert(sum_premvalues != 0.0);
  return sum_rulevalues / sum_premvalues;
}

// //////////////////////////////////////////////////////////////////////

Real 
FModel::R2(const Data& d) {
  assert(frules_.size() > 0);
  assert(d.y().size() > 0);
  if (d.variance_y() < 10*REAL_MIN) {
    string msg = (string)GLOBAL::prgname + ": error in FModel::R2(): ";
    msg += (string)"y variance of input data is (almost) 0.0\n";
    if ((!GLOBAL::quiet) || (!GLOBAL::logfile)) {
      cerr << msg << flush;
    }
    if (GLOBAL::logfile) {
      GLOBAL::logfile << msg;
    }
    return -REAL_MAX;
  }
  vector<Real> error_vect(d.y().size());
  vector<Uvector>::const_iterator u = d.U().begin();
  vector<Real>::const_iterator y = d.y().begin();
  vector<Real>::iterator error = error_vect.begin();
  Real mean_error = 0.0;
  Real variance_error = 0.0;
  while (u != d.U().end()) { // for all u
    Real yhat;
    try {
      yhat = y_hat(*u);
    }
    catch(IncompleteCoverageError& error) {
      string msg = (string)GLOBAL::prgname+": error caught in FModel::R2(): ";
      msg += error.msg() + "\n";
      if ((!GLOBAL::quiet) || (!GLOBAL::logfile)) {
	cerr << msg << flush;
      }
      if (GLOBAL::logfile) {
	GLOBAL::logfile << msg;
      }
      yhat = 0.0;
      //return -REAL_MAX;
    }
    *error = *y - yhat;
    mean_error += *error;
    ++u;
    ++y;
    ++error;
  } // end for all u
  mean_error /= d.y().size();
  error = error_vect.begin();
  while (error != error_vect.end()) {
    *error -= mean_error;
    variance_error += *error * *error;
    ++error;
  }
  variance_error /= d.y().size();
  return 1.0 - variance_error / d.variance_y();
}


// //////////////////////////////////////////////////////////////////////

size_t 
FModel::worst_rule_index(const Data& d) throw (Error) {
  assert(frules_.size() > 0);
  vector<Real> w(rdim_);
  vector<Real> f(rdim_);
  vector<Real> rule_error(rdim_, 0.0);
  vector<Uvector>::const_iterator u = d.U().begin();
  vector<Real>::const_iterator y = d.y().begin();
  // ///// calculate forward step
  while (u != d.U().end()) { // //// for all u
    Real sum_w = 0.0;
    Real y_hat = 0.0;
    Real difference;
    Real error = 0.0;
    FRuleContainer::const_iterator pr = frules_.begin();
    vector<Real>::iterator pw = w.begin();
    vector<Real>::iterator pf = f.begin();
    // /// forward step: calculate w, f, error
    while (pr != frules_.end()) { // /// for all rules 
      assert(pr != frules_.end());
      assert(pw != w.end());
      assert(pf != f.end());
      *pw = pr->premvalue(*u);
      sum_w += *pw;
      *pf = pr->consvalue(*u);
      y_hat += inference(*pw, *pf);
      ++pr;
      ++pw;
      ++pf;
    } // /// end for all rules 
    if (sum_w <= 0.0) {
      throw IncompleteCoverageError("FModel::worst_rule_index()");
    }
    y_hat /= sum_w;
    difference = (*y - y_hat); 
    // /// calculate gradients for all parameters
    if (GLOBAL::norm == 2) {
      error = difference * difference;
    } 
    else if (GLOBAL::norm == 1) {
      error = sign(difference);
    }
    else if (GLOBAL::norm == 3) {
      error = sign(difference) * difference * difference;
    }
    else {
      assert(1==0);
    }
    vector<Real>::iterator pe = rule_error.begin();
    pw = w.begin();
    while (pe != rule_error.end()) {
      *(pe++) += error * *(pw++);
    }
    ++u;
  }
  // ///// determine rule with biggest error
  Real error_max = -1.0;
  size_t index_max = 0;
  size_t index = 0;
  vector<Real>::const_iterator pe = rule_error.begin();
  while (pe != rule_error.end()) {
    if (*pe > error_max) {
      index_max = index;
      error_max = *pe;
    }
    ++pe;
    ++index;
  }
  worst_rule_ = index_max;
  return index_max;
}

// //////////////////////////////////////////////////////////////////////

Real
FModel::estimation(const Data& d, const char* outfilename) throw (Error) {
  assert(frules_.size() > 0);
//   struct timeval time_begin;
//   struct timeval time_end;
//   gettimeofday(&time_begin, NULL);
  ofstream file;
  if (outfilename != NULL) {
    file.open(outfilename);
    if (!file) {
      throw FileOpenError(outfilename);
    }
    file << "### data file: \"" << d.filename() << "\"\n" << flush;
  }
  Real sum_error = 0.0;
  vector<Uvector>::const_iterator u = d.U().begin();
  vector<Real>::const_iterator y = d.y().begin();
  size_t blockline = 0;
  // /// for Page-Hinkley test
  Real U_t = 0.0;
  Real T_t = 0.0;
  Real m_t = 0.0;
  Real M_t = 0.0;
  // /// do the estimation
  while (u != d.U().end()) { // //// for all u
    ++blockline;
    Real yhat;
    try {
      yhat = y_hat(*u);
    }
    catch(IncompleteCoverageError& error) {
      string msg = (string)GLOBAL::prgname; 
      msg += (string)": warning from FModel::estimation(): "+error.msg()+"\n";
      if ((!GLOBAL::quiet) || (!GLOBAL::logfile)) {
	cerr << msg << flush;
      }
      if (GLOBAL::logfile) {
	GLOBAL::logfile << msg;
      }
      if (outfilename != NULL) {
	file << " ### warning: next u not covered by model:\n";
      }
      yhat = 0.0;
    }
    if (outfilename != NULL) {
      Real difference = 0.0;
      if (GLOBAL::denormalize) {
	Uvector::const_iterator pu = u->begin();
	vector<Real>::const_iterator ps = d.scale_shift().begin();
	vector<Real>::const_iterator pf = d.scale_factor().begin();
	while (pu != u->end()) {
	  // /// print u
	  file << denormalization(*(pu++), *(pf++), *(ps++)) << " ";
	}
	// /// print yhat
	file << denormalization(yhat, *(pf), *ps) << " ";
	// /// print y
	file << denormalization(*y, *(pf), *ps) << " ";
	// /// print y - yhat
	difference = denormalization( (*y - yhat) , *(pf), err_offset);
	file << (difference + GLOBAL::error_offset);
      }
      else {
	Uvector::const_iterator pu = u->begin();
	while (pu != u->end()) {
	  // /// print u
	  file << *(pu++) << " ";
	}
	// /// print yhat
	file << yhat << " ";
	// /// print y
	file << *y << " ";
	// /// print y - yhat
	difference = (*y - yhat);
	file << (difference + GLOBAL::error_offset);
      }
      // /// Page-Hinkley test
      if (GLOBAL::page_hinkley) {
	U_t += difference - GLOBAL::page_hinkley_mu_0 
	  - 0.5 * GLOBAL::page_hinkley_nu_inc;
	T_t += difference - GLOBAL::page_hinkley_mu_0 
	  + 0.5 * GLOBAL::page_hinkley_nu_dec;
	if (U_t < m_t) {
	  m_t = U_t;
	}
	if (T_t > M_t) {
	  M_t = T_t;
	}
	file << " " << (U_t - m_t);
	file << " " << (M_t - T_t);
	if ( ((U_t - m_t) >= GLOBAL::page_hinkley_lambda)
	     || ((M_t - T_t) >= GLOBAL::page_hinkley_lambda) ) { 
	  file << " 1";
	}
	else {
	  file << " 0";
	}
      }
      file << endl;
      // /// print additional newlines
      if (blockline == d.blocksize()) {
	file << endl;
	blockline = 0;
      }
    }
    Real y_scale_factor =  *(d.scale_factor().end() - 1);
    Real error = *y - yhat;
    if (y_scale_factor > 0.0) {
      error /= y_scale_factor;
    }
    sum_error += error * error;
    ++u;
    ++y;
  } // //// end for all u
  if (outfilename != NULL) {
    file.close();
  }
//   gettimeofday(&time_end, NULL);
//   long time_diff = 1000000 * (time_end.tv_sec - time_begin.tv_sec)
//     + (time_end.tv_usec - time_begin.tv_usec);
//   if (outfilename == NULL) {
//     verbose(2, "estimation time", time_diff);
//   }
  return sqrt(sum_error / d.U().size());
  // return sqrt(sum_error);
}


// //////////////////////////////////////////////////////////////////////

Real
FModel::simulation(const Data& d, const char* outfilename) throw (Error) {
  assert(frules_.size() > 0);
  assert(d.U().size() > 0);
  assert(d.U()[0].size() > 0);
  assert(GLOBAL::order > 0);
  assert(GLOBAL::order < d.U()[0].size());
  if (GLOBAL::order <= 0) {
    return REAL_MAX;
  }
  if (GLOBAL::order >= d.U()[0].size()) {
    return REAL_MAX;
  }
  ofstream file;
  if (outfilename != NULL) {
    file.open(outfilename);
    if (!file) {
      throw FileOpenError(outfilename);
    }
    file << "### data file: \"" << d.filename() << "\"\n" << flush;
  }
  Real sum_error = 0.0;
  vector<Uvector>::const_iterator u = d.U().begin();
  vector<Real>::const_iterator y = d.y().begin();
  Uvector recurrent_u(*u);
  size_t blockline = 0;
  Real yhat = 0.0;
  // /// for Page-Hinkley test
  Real U_t = 0.0;
  Real T_t = 0.0;
  Real m_t = 0.0;
  Real M_t = 0.0;
  // /// do the simulation
  while (u != d.U().end()) {
    // //// for all u
    ++blockline;
    Uvector::const_iterator pu = u->end() - 1;
    Uvector::iterator precurrent_u = recurrent_u.end() - 1;
    if (u != d.U().begin()) {
      // /// insert recurrency, except at first step
      while (precurrent_u != recurrent_u.end() - GLOBAL::order) {
	*precurrent_u = *(precurrent_u - 1);
	precurrent_u--;
      }
      // /// copy last y_hat
      *(precurrent_u--) = yhat;
      // /// copy rest of u
      pu -= GLOBAL::order;
      while (pu >= u->begin()) {
	*(precurrent_u--) = *(pu--);
      } 
    }
    else {
      // /// copy u
      while (pu >= u->begin()) {
	*(precurrent_u--) = *(pu--);
      } 
    }
    try {
      yhat = y_hat(recurrent_u);
    }
    catch(IncompleteCoverageError& error) {
      string msg = (string)GLOBAL::prgname; 
      msg += (string)": warning from FModel::simulation(): "+error.msg()+"\n";
      if ((!GLOBAL::quiet) || (!GLOBAL::logfile)) {
	cerr << msg << flush;
      }
      if (GLOBAL::logfile) {
	GLOBAL::logfile << msg;
      }
      if (outfilename != NULL) {
	file << " ### warning: next u not covered by model:\n";
      }
      yhat = 0.0;
    }
    if (outfilename != NULL) {
      Real difference = 0.0;
      if (GLOBAL::denormalize) {
	// /// print recurrent_u
	precurrent_u = recurrent_u.begin();
	vector<Real>::const_iterator ps = d.scale_shift().begin();
	vector<Real>::const_iterator pf = d.scale_factor().begin();
	while (precurrent_u != recurrent_u.end()) {
	  file << denormalization(*(precurrent_u++), *(pf++), *(ps++)) << " ";
	}
	// /// print yhat
	file << denormalization(yhat, *(pf), *ps) << " ";
	// /// print y
	file << denormalization(*y, *(pf), *ps) << " ";
	// /// print y - yhat
	difference = denormalization( (*y - yhat) , *(pf), err_offset);
	file << (difference + GLOBAL::error_offset);
      }
      else {
	// /// print recurrent_u
	precurrent_u = recurrent_u.begin();
	while (precurrent_u != recurrent_u.end()) {
	  file << *(precurrent_u++) << " ";
	}
	// /// print yhat
	file << yhat << " ";
	// /// print y
	file << *y << " ";
	// /// print y - yhat
	difference = (*y - yhat);
	file << (difference + GLOBAL::error_offset);
      }
      // /// Page-Hinkley test
      if (GLOBAL::page_hinkley) {
	U_t += difference - GLOBAL::page_hinkley_mu_0 
	  - 0.5 * GLOBAL::page_hinkley_nu_inc;
	T_t += difference - GLOBAL::page_hinkley_mu_0 
	  + 0.5 * GLOBAL::page_hinkley_nu_dec;
	if (U_t < m_t) {
	  m_t = U_t;
	}
	if (T_t > M_t) {
	  M_t = T_t;
	}
	file << " " << (U_t - m_t);
	file << " " << (M_t - T_t);
	if ( ((U_t - m_t) >= GLOBAL::page_hinkley_lambda)
	     || ((M_t - T_t) >= GLOBAL::page_hinkley_lambda) ) { 
	  file << " 1";
	}
	else {
	  file << " 0";
	}
      }
      file << endl;
      // /// print additional newlines
      if (blockline == d.blocksize()) {
	file << endl;
	blockline = 0;
      }
    }
    Real y_scale_factor =  *(d.scale_factor().end() - 1);
    Real error = *y - yhat;
    if (y_scale_factor > 0.0) {
      error /= y_scale_factor;
    }
    sum_error += error * error;
    ++u;
    ++y;
  } // //// end for all u
  if (outfilename != NULL) {
    file.close();
  }
  return sqrt(sum_error / d.U().size());
  // return sqrt(sum_error);
}


// //////////////////////////////////////////////////////////////////////

void 
FModel::reset_consequences(void) {
  // /// see also: RPROP_init()
  // / initialize consequences
  FRuleContainer::const_iterator pr = frules_.begin();
  while (pr != frules_.end()) { // /// for all rules
    Consequence::iterator p_cons = pr->cons().begin();
//     Consequence::iterator pd_cons = pr->d_cons().begin();
//     Consequence::iterator pd_old_cons = pr->d_old_cons().begin();
//     Consequence::iterator p_delta = pr->delta_cons().begin();
    while (p_cons != pr->cons().end()) {
      *(p_cons++) = 0.0;
//       *(pd_cons++) = 0.0;
//       *(pd_old_cons++) = 0.0;
//       *(p_delta) = delta_cons_0;
    }
    ++pr;
  } // /// end all rules
  return;
}

// //////////////////////////////////////////////////////////////////////

void 
FModel::optimize_SVD(const Data& d) throw (Error) {
  assert(frules_.size() > 0);
  assert(d.U().size() == d.y().size());
  size_t m = d.U().size();
  size_t n = cdim_ * rdim_;
  assert(m >= n);
  Vector<Real> p(n);
  Matrix<Real> A(m, n);
  Matrix<Real> Q;
  Matrix<Real> U;
  Matrix<Real> V;
  // create y
  Vector<Real> y(m);
  Vector<Real>::iterator py = y.begin();
  vector<Real>::const_iterator pdy = d.y().begin();
  while (pdy != d.y().end()) {
    *(py++) = *(pdy++);
  }
  // /// build A
  vector<Uvector>::const_iterator u = d.U().begin();
  size_t row = 1;
  while (u != d.U().end()) { // for all u
    Real sum_w = 0.0;
    vector<Real> w(rdim_);
    vector<Real>::iterator pw = w.begin();
    FRuleContainer::const_iterator pr = frules_.begin();
    while (pr != frules_.end()) { // for all rules 
      assert(pr != frules_.end());
      assert(pw != w.end());
      *pw = pr->premvalue(*u);
      sum_w += *pw;
      ++pr;
      ++pw;
    } // end for all rules 
    if (sum_w <= 0.0) {
      throw IncompleteCoverageError("FModel::optimize_SVD()");
    }
    pw = w.begin();
    pr = frules_.begin();
    size_t column = 1;
    while (pr != frules_.end()) { // for all rules 
      Real v = *pw / sum_w;
      A(row, column) = v; // * 1.0; // c_0
      ++column;
      Uvector::const_iterator ui = u->begin();
      for(size_t c = 1; c < cdim_; ++c) { // c_1, ..., c_C (max c_N)
	A(row, column++) = v * *(ui++);
      }
      ++pr;
      ++pw;
    } // end for all rules 
    ++u;
    ++row;
  } // end for all u
  // /// solve y = A * p
  try {
    SVD(A, Q, U, V);
  }
  catch(SVDError& error) {
    string msg = (string)"\n" + GLOBAL::prgname + ": ";
    msg += (string)"optimize_SVD(): warning: " + error.msg();
    msg += (string)"\nSVD optimization of consequence parameters skipped.\n";
    if ((!GLOBAL::quiet) || (!GLOBAL::logfile)) {
      cerr << msg << flush;
    }
    if (GLOBAL::logfile) {
      GLOBAL::logfile << msg;
    }
    return;
  }
  // calculate inverse of Q
  for(row = 1; row <= n; ++row) {
    if (Q(row, row) != 0.0) {
      Q(row, row) = 1.0 / Q(row, row);
    }
  }
  // p is LMS solution of y = A * p;
  p = V * (Q * (transpose(U) * y)); 
  // /// write p to consequence parameters
  FRuleContainer::const_iterator pr = frules_.begin();
  Vector<Real>::const_iterator pp = p.begin();
  while (pr != frules_.end()) { // for all rules 
    Consequence::iterator pc = pr->cons().begin();
    while (pc != pr->cons().end()) {
      *(pc++) = *(pp++);
    }
    ++pr;
  } // end for all rules 
}

// //////////////////////////////////////////////////////////////////////

Real 
FModel::optimize_GRAD_DESCENT(const Data& a, const Data& b, 
		       size_t min_iterations, size_t max_iterations) {
  assert(frules_.size() > 0);
  // / cross validate after each N_OPTS_PER_STEP iterations of GRAD_DESCENT
  size_t iteration = 0;
  Real a_fitness = REAL_MAX;
  Real b_fitness = REAL_MAX;
  Real sum_a_fitness = REAL_MAX;  
  Real sum_b_fitness = REAL_MAX;
  struct timeval time_begin;
  struct timeval time_end;
  gettimeofday(&time_begin, NULL);
  while (iteration < max_iterations) {
    try {
      Real new_a_fitness = 0.0;
      Real new_b_fitness = 0.0;
      Real new_sum_a_fitness = 0.0;
      Real new_sum_b_fitness = 0.0;
      size_t k = 0;
      while (k < GLOBAL::steps_per_validation) {
	new_a_fitness = GRAD_DESCENT(a);
	new_b_fitness = estimation(b); // hold out method (cross validation)
	new_sum_a_fitness += new_a_fitness;
	new_sum_b_fitness += new_b_fitness; 
	++iteration;
	++k;
      }
      //new_b_fitness = estimation(b);
      if (iteration < min_iterations)  {
	// do further optimize (in any case)
      }
      else { // check for success
	if ( (new_sum_a_fitness < sum_a_fitness) 
	     && (new_sum_b_fitness < sum_b_fitness) ) {
	  // / success: do further optimize
	  a_fitness = new_a_fitness;
	  b_fitness = new_b_fitness;
	  sum_a_fitness = new_sum_a_fitness;
	  sum_b_fitness = new_sum_b_fitness;
	}
	else {
	  // /// take back last update step
	  GRAD_DESCENT_backstep();
	  --iteration;
	  // /// stop optimization
	  break;
	}
      }
    } // end try
    catch(IncompleteCoverageError& error) {
      string msg = (string)GLOBAL::prgname;
      msg+=(string)": warning from optimize_GRAD_DESCENT(): "+error.msg()+"\n";
      if ((!GLOBAL::quiet) || (!GLOBAL::logfile)) {
	cerr << msg << flush;
      }
      if (GLOBAL::logfile) {
	GLOBAL::logfile << msg;
      }
      GRAD_DESCENT_backstep();
      --iteration;
      break;
    } // end catch
  } // end while (iteration < max_iterations) 
  gettimeofday(&time_end, NULL);
  long time_diff = 1000000 * (time_end.tv_sec - time_begin.tv_sec)
    + (time_end.tv_usec - time_begin.tv_usec);
  long mean_iteration_time = time_diff / (long)iteration;
  verbose(2, "GRAD_DESCENT iterations", iteration);
  verbose(2, "GRAD_DESCENT mean time per iteration", mean_iteration_time);
  verbose(20, "fitness on training data:", a_fitness);   //#WIN2017 
  verbose(20, "fitness on validation data:", b_fitness); //#WIN2017 
  return b_fitness;
}

// //////////////////////////////////////////////////////////////////////

Real 
FModel::GRAD_DESCENT(const Data& d) throw (Error) {
  assert(frules_.size() > 0);
  vector<Real> w(rdim_);
  vector<Real> f(rdim_);
  Real sum_error = 0.0;
  vector<Uvector>::const_iterator u = d.U().begin();
  vector<Real>::const_iterator y = d.y().begin();
  // ///// calculate forward step and new gradients 
  // ///// for each u individually
  while (u != d.U().end()) { // //// for all u
    Real sum_w = 0.0;
    Real y_hat = 0.0;
    Real difference;
    FRuleContainer::const_iterator pr = frules_.begin();
    vector<Real>::iterator pw = w.begin();
    vector<Real>::iterator pf = f.begin();
    // /// forward step: calculate w, f, error
    while (pr != frules_.end()) { // /// for all rules 
      assert(pr != frules_.end());
      assert(pw != w.end());
      assert(pf != f.end());
      *pw = pr->premvalue(*u);
      sum_w += *pw;
      *pf = pr->consvalue(*u);
      y_hat += inference(*pw, *pf);
      ++pr;
      ++pw;
      ++pf;
    } // /// end for all rules 
    if (sum_w <= 0.0) {
      throw IncompleteCoverageError("FModel::GRAD_DESCENT()");
    }
    y_hat /= sum_w;
    difference = (*y - y_hat); 
    sum_error += difference * difference;
    // /// calculate gradients for all parameters
    Real factor = 1.0;
    if (GLOBAL::norm == 2) {
      factor = difference / sum_w;
    } 
    else if (GLOBAL::norm == 1) {
      factor = sign(difference) / sum_w;
    }
    else if (GLOBAL::norm == 3) {
      factor = sign(difference) * difference * difference / sum_w;
    }
    else {
      assert(1==0);
      throw Error("fatal error in FModel::GRAD_DESCENT: invalid GLOBAL::norm");
    }
    // /// factor for cons
    Real factor_c = factor;
    for (int i=0; i<GLOBAL::local_cons_optimization; ++i) {
      factor_c /= sum_w;
    }
    // / 1. calculate consequence parameter gradients
    pr = frules_.begin();
    pw = w.begin();
    pf = f.begin();
    while (pr != frules_.end()) { // /// for all rules
      assert(pr != frules_.end());
      assert(pw != w.end());
      assert(pf != f.end());
      // /// for all rules: calculate parameter gradients if w_r > 0
      if (*pw > 0.0) { // / i.e., if w[r] > 0
	// / 1. calculate consequence parameter gradients
	register Real factor_r = -factor_c * *pw;
	for (int i=0; i<GLOBAL::local_cons_optimization; ++i) {
	  factor_r *= *pw;
	}
	Consequence::iterator pc = pr->d_cons().begin();
	Uvector::const_iterator pu = u->begin();
	*(pc++) += factor_r; // * 1.0;     // / c0
	while (pc != pr->d_cons().end()) { // / c1...cN
	  *(pc++) += factor_r * *(pu++);
	}
      } // / end if (w[r] > 0)
      // / else: nothing to do (add 0.0 to all gradients)
      ++pr;
      ++pw;
      ++pf;
    } // /// end for all rules 
    // / 2. calculate fset parameter gradients
    FSetContainer::iterator ps = fsets_.begin();
    while (ps != fsets_.end()) { // /// for all fsets
      Real sum_w_d = 0.0;
      Real sum_fw_d = 0.0;
      pr = frules_.begin();
      pw = w.begin();
      pf = f.begin();
      int uindex = -1;
      while (pr != frules_.end()) { // /// for all rules
	// if (*pw > 0.0) { // / i.e., if w[r] > 0.0
	Premise::const_iterator pprem = pr->prem().begin();	
	assert(pprem != pr->prem().end());
	do { // /// for all clauses in rule r
	  //#WIN2017 if (*pprem == ps) { // fset ps in premise of rule pr
	  if (*pprem == &*ps) { // fset ps in premise of rule pr
	    uindex = (pprem - pr->prem().begin()) / 2;
	    sum_w_d += *pw;
	    sum_fw_d += *pf * *pw;
	  }
	  ++pprem;
	} while (pprem != pr->prem().end());
	// } // /// end if w[r] > 0.0
	++pr;
	++pw;
	++pf;
      } // /// end for all rules
      // /// compute common factor
      assert(uindex >= 0);
      assert(uindex < (int)udim_);
      Real factor_s = factor * (y_hat*sum_w_d - sum_fw_d);
#ifdef TRAPEZOIDAL_FSETS
      Real fset_value = 0.0;
      int odd_fset = 0;
      FSetContainer::iterator ps_old = fsets_.begin();
      Real factor_s_old = 0.0;
      Real left_old = 0.0;
      Real right_old = 0.0;
      Real fset_value_old = 0.0;
      if (ps->sigma() != 0.0) {
	Real left;
	Real right;
	if (ps->sigma() > 0.0) {
	  left = ps->mu() - 0.5 / ps->sigma();
	  right = ps->mu() + 0.5 / ps->sigma();
	}
	else {
	  left = ps->mu() + 0.5 / ps->sigma();
	  right = ps->mu() - 0.5 / ps->sigma();
	}
	if ( ((*u)[uindex] > left) && ((*u)[uindex] < right) ) {
	  fset_value = 1.0 / ps->F((*u)[uindex]);
	  ps->add_d_sigma(factor_s * fset_value * (((*u)[uindex]) - ps->mu()));
	}
	// else: derivation is 0
	// /// check for equal adjacent mu
	if (GLOBAL::adjacent_equal_mu) {
	  if (odd_fset) {
	    if ( ((*u)[uindex] > left) && ((*u)[uindex] < right)
		 && ((*u)[uindex] > left_old) && ((*u)[uindex] < right_old) ) {
	      fset_value = 1.0 / ps->F((*u)[uindex]);
	      Real delta_mu = (factor_s + factor_s_old) 
		* fset_value *  fset_value_old
		* (- ps->sigma()) * (- ps_old->sigma());
	      ps->add_d_mu(delta_mu);
	      ps_old->add_d_mu(delta_mu);
	    }
	    odd_fset = 0;
	  }
	  else {
	    odd_fset = 1;
	    ps_old = ps;
	    factor_s_old = factor_s;
	    left_old = left;
	    right_old = right;
	    fset_value_old = fset_value;
	  }
	}
	else {
	  if ( ((*u)[uindex] > left) && ((*u)[uindex] < right) ) {
	    ps->add_d_mu(factor_s * fset_value * ( - ps->sigma()));
	  }
	  // else: derivation is 0
	}
      }
      else { 
	throw Error("error in FModel::GRAD_DESCENT(): div through sigma == 0");
      }
#else
    int odd_fset = 0;
    FSetContainer::iterator ps_old = fsets_.begin();
    Real factor_s_old = 0.0;
    Real fset_value_old = 0.0;
      Real fset_value = (1.0 - ps->F((*u)[uindex]));
      ps->add_d_sigma(factor_s * fset_value * (ps->mu() - (*u)[uindex]));
      // /// check for equal adjacent mu
      if (GLOBAL::adjacent_equal_mu) {
	if (odd_fset) {
	  odd_fset = 0;
	  Real delta_mu =  (factor_s + factor_s_old)
	    * fset_value * fset_value_old 
	    * ps->sigma() * ps_old->sigma();
	  ps->add_d_mu(delta_mu);
	  ps_old->add_d_mu(delta_mu);
	}
	else {
	  odd_fset = 1;
	  ps_old = ps;
	  factor_s_old = factor_s;
	  fset_value_old = fset_value;
	}
      }
      else {
	ps->add_d_mu(factor_s * fset_value * ps->sigma());
      }
#endif
      ++ps;
    } // /// end for all fsets 
    ++u;
    ++y;
  } // /// end for all u
  // ***** update parameters using GRAD_DESCENT rule
  // / 1. update consequence parameters
  FRuleContainer::const_iterator pr = frules_.begin();
  if (GLOBAL::consequence_optimize_SVD) {
    optimize_SVD(d);
  }
  else {
    while (pr != frules_.end()) { // /// for all rules: 
      Consequence::iterator pcons = pr->cons().begin();
      Consequence::iterator pd_cons = pr->d_cons().begin();
      Consequence::iterator p_delta = pr->delta_cons().begin();
      while (pcons != pr->cons().end()) {
	Real new_delta = GLOBAL::beta * *p_delta - GLOBAL::alpha * *pd_cons;
	*pcons += new_delta;
	*p_delta = new_delta;
	*pd_cons = 0.0;
	++pcons;
	++pd_cons;
	++p_delta;
      }
      ++pr;
    } // /// end for all rules
  }
  // / 2. update fset parameters
  if (! GLOBAL::update_premise) {
    return sqrt(sum_error);
  }
  FSetContainer::iterator ps = fsets_.begin();
  while (ps != fsets_.end()) { // /// for all fsets
    Real new_delta_mu = GLOBAL::beta * ps->delta_mu()
      - GLOBAL::alpha * ps->d_mu();
    ps->add_mu(new_delta_mu);
    ps->delta_mu() = new_delta_mu;
    ps->d_mu() = 0.0;
    Real new_delta_sigma = GLOBAL::beta * ps->delta_sigma()
      - 20.0 * GLOBAL::alpha * ps->d_sigma();
    ps->add_sigma(new_delta_sigma);
    ps->delta_sigma() = new_delta_sigma;
    ps->d_sigma() = 0.0;
    ++ps;
  } // /// end for all fsets 
  return sqrt(sum_error);
}

// //////////////////////////////////////////////////////////////////////

void 
FModel::GRAD_DESCENT_init(void) {
  // / 1. initialize consequences
  FRuleContainer::const_iterator pr = frules_.begin();
  while (pr != frules_.end()) { // /// for all rules
    Consequence::iterator pd_cons = pr->d_cons().begin();
    Consequence::iterator pd_old_cons = pr->d_old_cons().begin();
    Consequence::iterator p_delta = pr->delta_cons().begin();
    while (pd_cons != pr->d_cons().end()) {
      *(pd_cons++) = 0.0;
      *(pd_old_cons++) = 0.0;
      *(p_delta++) = 0.0;
    }
    ++pr;
  } // /// end all rules
  // / 2. initialize fsets
  FSetContainer::iterator ps = fsets_.begin();
  while (ps != fsets_.end()) { // /// for all fsets
    ps->d_mu() = 0.0;
    ps->d_old_mu() = 0.0;
    ps->delta_mu() = 0.0;
    ps->d_sigma() = 0.0;
    ps->d_old_sigma() = 0.0;
    ps->delta_sigma() = 0.0;
    ++ps;
  } // /// end for all fsets 
  return;
}

// //////////////////////////////////////////////////////////////////////

void 
FModel::GRAD_DESCENT_backstep(void) {
#ifdef TRAPEZOIDAL_FSETS
  return;
#endif
  // / 1. take back the consequence parameters' update
  FRuleContainer::const_iterator pr = frules_.begin();
  while (pr != frules_.end()) { // /// for all rules
    Consequence::iterator pcons = pr->cons().begin();
    Consequence::iterator p_delta = pr->delta_cons().begin();
    while (pcons != pr->cons().end()) {
      *pcons -= *p_delta;
      ++pcons;
      ++p_delta;
    }
    ++pr;
  } // /// end all rules
  // / 2. take back the fset parameters' update
  if (! GLOBAL::update_premise) {
    return;
  }

  FSetContainer::iterator ps = fsets_.begin();
  while (ps != fsets_.end()) { // /// for all fsets
    ps->add_mu( -ps->delta_mu() );
    ps->add_sigma( -ps->delta_sigma() );
    ++ps;
  } // /// end for all fsets 
  return;
}

// //////////////////////////////////////////////////////////////////////

Real 
FModel::optimize_RPROP(const Data& a, const Data& b, 
		       size_t min_iterations, size_t max_iterations) {
  assert(frules_.size() > 0);
  // / cross validate after each N_OPTS_PER_STEP iterations of RPROP
  size_t iteration = 0;
  Real a_fitness = REAL_MAX;
  Real b_fitness = REAL_MAX;
  Real sum_a_fitness = REAL_MAX;  
  Real sum_b_fitness = REAL_MAX;
  struct timeval time_begin;
  struct timeval time_end;
  gettimeofday(&time_begin, NULL);
  while (iteration < max_iterations) {
    try {
      Real new_a_fitness = 0.0;
      Real new_b_fitness = 0.0;
      Real new_sum_a_fitness = 0.0;
      Real new_sum_b_fitness = 0.0;
      size_t k = 0;
      while (k < GLOBAL::steps_per_validation) {
	new_a_fitness = RPROP(a);
	new_b_fitness = estimation(b); // hold out method (cross validation)
	new_sum_a_fitness += new_a_fitness;
	new_sum_b_fitness += new_b_fitness; 
	++iteration;
	++k;
      }
      //new_b_fitness = estimation(b);
      if (iteration < min_iterations)  {
	// do further optimize (in any case)
      }
      else { // check for success
	if ( (new_sum_a_fitness < sum_a_fitness) 
	     && (new_sum_b_fitness < sum_b_fitness) ) {
	  // / success: do further optimize
	  a_fitness = new_a_fitness;
	  b_fitness = new_b_fitness;
	  sum_a_fitness = new_sum_a_fitness;
	  sum_b_fitness = new_sum_b_fitness;
	}
	else {
	  // /// take back last update step
	  RPROP_backstep();
	  --iteration;
	  // /// stop optimization
	  break;
	}
      }
    } // end try
    catch(IncompleteCoverageError& error) {
      string msg = (string)GLOBAL::prgname;
      msg += (string)": warning from optimize_RPROP(): " + error.msg() + "\n";
      if ((!GLOBAL::quiet) || (!GLOBAL::logfile)) {
	cerr << msg << flush;
      }
      if (GLOBAL::logfile) {
	GLOBAL::logfile << msg;
      }
      RPROP_backstep();
      --iteration;//<<<=== creates compile error on old gcc; if so, comment out
      break;
    } // end catch
    catch(...) {
      string msg = "fatal error: unknown error caught in optimize_RPROP()\n";
      cerr << msg << flush;
      exit(-1);
    }
  } // end while (iteration < max_iterations) 
  gettimeofday(&time_end, NULL);
  long time_diff = 1000000 * (time_end.tv_sec - time_begin.tv_sec)
    + (time_end.tv_usec - time_begin.tv_usec);
  long mean_iteration_time = time_diff / (long)iteration;
  verbose(2, "RPROP iterations", iteration);
  verbose(2, "RPROP mean time per iteration", mean_iteration_time);
  verbose(20, "fitness on training data:", a_fitness);   //#WIN2017 
  verbose(20, "fitness on validation data:", b_fitness); //#WIN2017 
  return b_fitness;
}

// //////////////////////////////////////////////////////////////////////

Real 
FModel::RPROP(const Data& d) throw (Error) {
  assert(frules_.size() > 0);
  vector<Real> w(rdim_);
  vector<Real> f(rdim_);
  Real sum_error = 0.0;
  vector<Uvector>::const_iterator u = d.U().begin();
  vector<Real>::const_iterator y = d.y().begin();
  // ///// calculate forward step and new gradients 
  // ///// for each u individually
  while (u != d.U().end()) { // //// for all u
    Real sum_w = 0.0;
    Real y_hat = 0.0;
    Real difference;
    FRuleContainer::const_iterator pr = frules_.begin();
    vector<Real>::iterator pw = w.begin();
    vector<Real>::iterator pf = f.begin();
    // /// forward step: calculate w, f, error
    while (pr != frules_.end()) { // /// for all rules 
      assert(pr != frules_.end());
      assert(pw != w.end());
      assert(pf != f.end());
      *pw = pr->premvalue(*u);
      sum_w += *pw;
      *pf = pr->consvalue(*u);
      y_hat += inference(*pw, *pf);
      ++pr;
      ++pw;
      ++pf;
    } // /// end for all rules 
    if (sum_w <= 0.0) {
      throw IncompleteCoverageError("FModel::RPROP()");
    }
    y_hat /= sum_w;
    difference = (*y - y_hat); 
    sum_error += difference * difference;
    // /// calculate gradients for all parameters
    Real factor = 1.0;
    if (GLOBAL::norm == 2) {
      factor = difference / sum_w;
    } 
    else if (GLOBAL::norm == 1) {
      factor = sign(difference) / sum_w;
    }
    else if (GLOBAL::norm == 3) {
      factor = sign(difference) * difference * difference / sum_w;
    }
    else {
      assert(1==0);
      throw Error("fatal error in FModel::RPROP(): invalid GLOBAL::norm");
    }
    // /// factor for cons
    Real factor_c = factor;
    for (int i=0; i<GLOBAL::local_cons_optimization; ++i) {
      factor_c /= sum_w;
    }
    // / 1. calculate consequence parameter gradients
    pr = frules_.begin();
    pw = w.begin();
    pf = f.begin();
    while (pr != frules_.end()) { // /// for all rules
      assert(pr != frules_.end());
      assert(pw != w.end());
      assert(pf != f.end());
      // /// for all rules: calculate parameter gradients if w_r > 0
      if (*pw > 0.0) { // / i.e., if w[r] > 0
	// / 1. calculate consequence parameter gradients
	register Real factor_r = -factor_c * *pw;
	for (int i=0; i<GLOBAL::local_cons_optimization; ++i) {
	  factor_r *= *pw;
	}
	Consequence::iterator pc = pr->d_cons().begin();
	Uvector::const_iterator pu = u->begin();
	*(pc++) += factor_r; // * 1.0;     // / c0
	while (pc != pr->d_cons().end()) { // / c1...cN
	  *(pc++) += factor_r * *(pu++);
	}
      } // / end if (w[r] > 0)
      // / else: nothing to do (add 0.0 to all gradients)
      ++pr;
      ++pw;
      ++pf;
    } // /// end for all rules 
    // / 2. calculate fset parameter gradients
    FSetContainer::iterator ps = fsets_.begin();
    while (ps != fsets_.end()) { // /// for all fsets
      Real sum_w_d = 0.0;
      Real sum_fw_d = 0.0;
      pr = frules_.begin();
      pw = w.begin();
      pf = f.begin();
      int uindex = -1;
      while (pr != frules_.end()) { // /// for all rules
	// if (*pw > 0.0) { // / i.e., if w[r] > 0.0
	Premise::const_iterator pprem = pr->prem().begin();	
	assert(pprem != pr->prem().end());
	do { // /// for all clauses in rule r
	  //#WIN2017 if (*pprem == ps) { // fset ps in premise of rule pr
	  if (*pprem == &*ps) { // fset ps in premise of rule pr
	    uindex = (pprem - pr->prem().begin()) / 2;
	    sum_w_d += *pw;
	    sum_fw_d += *pf * *pw;
	  }
	  ++pprem;
	} while (pprem != pr->prem().end());
	// } // /// end if w[r] > 0.0
	++pr;
	++pw;
	++pf;
      } // /// end for all rules
      // /// compute common factor
      assert(uindex >= 0);
      assert(uindex < (int)udim_);
      Real factor_s = factor * (y_hat*sum_w_d - sum_fw_d);
#ifdef TRAPEZOIDAL_FSETS
//       if (ps->sigma() != 0.0) {
// 	Real left;
// 	Real right;
// 	if (ps->sigma() > 0.0) {
// 	  left = ps->mu() - 0.5 / ps->sigma();
// 	  right = ps->mu() + 0.5 / ps->sigma();
// 	}
// 	else {
// 	  left = ps->mu() + 0.5 / ps->sigma();
// 	  right = ps->mu() - 0.5 / ps->sigma();
// 	}
// 	if ( ((*u)[uindex] > left) && ((*u)[uindex] < right) ) {
// 	  factor_s *= 1.0 / ps->F((*u)[uindex]);
// 	  ps->add_d_mu(factor_s * ( - ps->sigma()));
// 	  ps->add_d_sigma(factor_s * ( - ps->mu() + (*u)[uindex]));
// 	}
// 	// else: derivation is 0
//       }
//       // else: derivation is 0
      Real fset_value = 0.0;
      int odd_fset = 0;
      FSetContainer::iterator ps_old = fsets_.begin();
      Real factor_s_old = 0.0;
      Real left_old = 0.0;
      Real right_old = 0.0;
      Real fset_value_old = 0.0;
      if (ps->sigma() != 0.0) {
	Real left;
	Real right;
	if (ps->sigma() > 0.0) {
	  left = ps->mu() - 0.5 / ps->sigma();
	  right = ps->mu() + 0.5 / ps->sigma();
	}
	else {
	  left = ps->mu() + 0.5 / ps->sigma();
	  right = ps->mu() - 0.5 / ps->sigma();
	}
	if ( ((*u)[uindex] > left) && ((*u)[uindex] < right) ) {
	  fset_value = 1.0 / ps->F((*u)[uindex]);
	  ps->add_d_sigma(factor_s * fset_value * (((*u)[uindex]) - ps->mu()));
	}
	// else: derivation is 0
	// /// check for equal adjacent mu
	if (GLOBAL::adjacent_equal_mu) {
	  if (odd_fset) {
	    if ( ((*u)[uindex] > left) && ((*u)[uindex] < right)
		 && ((*u)[uindex] > left_old) && ((*u)[uindex] < right_old) ) {
	      fset_value = 1.0 / ps->F((*u)[uindex]);
	      Real delta_mu = (factor_s + factor_s_old) 
		* fset_value *  fset_value_old
		* (- ps->sigma()) * (- ps_old->sigma());
	      ps->add_d_mu(delta_mu);
	      ps_old->add_d_mu(delta_mu);
	    }
	    odd_fset = 0;
	  }
	  else {
	    odd_fset = 1;
	    ps_old = ps;
	    factor_s_old = factor_s;
	    left_old = left;
	    right_old = right;
	    fset_value_old = fset_value;
	  }
	}
	else {
	  if ( ((*u)[uindex] > left) && ((*u)[uindex] < right) ) {
	    ps->add_d_mu(factor_s * fset_value * ( - ps->sigma()));
	  }
	  // else: derivation is 0
	}
      }
      else { 
	throw Error("error in FModel::RPROP(): division through sigma == 0");
      }
#else
      int odd_fset = 0;
      FSetContainer::iterator ps_old = fsets_.begin();
      Real factor_s_old = 0.0;
      Real fset_value_old = 0.0;
      Real fset_value = (1.0 - ps->F((*u)[uindex]));
      ps->add_d_sigma(factor_s * fset_value * (ps->mu() - (*u)[uindex]));
      // /// check for equal adjacent mu
      if (GLOBAL::adjacent_equal_mu) {
	if (odd_fset) {
	  odd_fset = 0;
	  Real delta_mu =  (factor_s + factor_s_old)
	    * fset_value * fset_value_old 
	    * ps->sigma() * ps_old->sigma();
	  ps->add_d_mu(delta_mu);
	  ps_old->add_d_mu(delta_mu);
	}
	else {
	  odd_fset = 1;
	  ps_old = ps;
	  factor_s_old = factor_s;
	  fset_value_old = fset_value;
	}
      }
      else {
	ps->add_d_mu(factor_s * fset_value * ps->sigma());
      }
#endif
      ++ps;
    } // /// end for all fsets 
    ++u;
    ++y;
  } // /// end for all u
  // ***** update parameters using RPROP rule
  // / 1. update consequence parameters
  FRuleContainer::const_iterator pr = frules_.begin();
  if (GLOBAL::consequence_optimize_SVD) {
    optimize_SVD(d);
  }
  else{
    while (pr != frules_.end()) { // /// for all rules: 
      Consequence::iterator pcons = pr->cons().begin();
      Consequence::iterator pd_cons = pr->d_cons().begin();
      Consequence::iterator pd_old_cons = pr->d_old_cons().begin();
      Consequence::iterator p_delta = pr->delta_cons().begin();
      while (pcons != pr->cons().end()) {
	if ( (*pd_cons * *pd_old_cons) >= 0.0 ) {
	  if  ( (*pd_cons * *pd_old_cons) > 0.0 ) {
	    *p_delta *= eta_plus;
	    limit_delta_cons(p_delta);
	  }
	  if (*pd_cons > 0.0) {
	    *pcons -= *p_delta;
	  }
	  else if (*pd_cons < 0.0) {
	    *pcons += *p_delta;
	  }
	  *pd_old_cons = *pd_cons;
	  *pd_cons = 0.0;	
	}
	else { // / if ( (*pd_cons * *pd_old_cons) < 0.0 ) 
	  if (*pd_old_cons > 0.0) {
	    *pcons += *p_delta;
	  }
	  else if (*pd_old_cons < 0.0) {
	    *pcons -= *p_delta;
	  }
	  *pd_old_cons = 0.0;
	  *pd_cons = 0.0;
	  *p_delta *= eta_minus;
	  limit_delta_cons(p_delta);
	}
	++pcons;
	++pd_cons;
	++pd_old_cons;
	++p_delta;
      }
      ++pr;
    } // /// end for all rules
  }
  // / 2. update fset parameters
  if (! GLOBAL::update_premise) {
    return sqrt(sum_error);
  }
  FSetContainer::iterator ps = fsets_.begin();
  while (ps != fsets_.end()) { // /// for all fsets
    if ( (ps->d_mu() * ps->d_old_mu()) >= 0.0 ) {
      if ( (ps->d_mu() * ps->d_old_mu()) > 0.0 ) {
	ps->mult_delta_mu(eta_plus);
      }
      if (ps->d_mu() > 0.0) {
	ps->add_mu( -ps->delta_mu() );
      }
      else if (ps->d_mu() < 0.0) {
	ps->add_mu( ps->delta_mu() );
      }
      ps->d_old_mu() = ps->d_mu();
      ps->d_mu() = 0.0;
    }
    else { // / if ( (ps->d_mu() * ps->d_old_mu()) < 0.0 ) 
      if (ps->d_old_mu() > 0.0) {
	ps->add_mu( ps->delta_mu() );
      }
      else if (ps->d_old_mu() < 0.0) {
	ps->add_mu( -ps->delta_mu() );
      }
      ps->mult_delta_mu(eta_minus);
      ps->d_old_mu() = 0.0;
      ps->d_mu() = 0.0;
    }
    if ( (ps->d_sigma() * ps->d_old_sigma()) >= 0.0 ) {
      if ( (ps->d_sigma() * ps->d_old_sigma()) > 0.0 ) {
	ps->mult_delta_sigma(eta_plus);
      }
      if (ps->d_sigma() > 0.0) {
	ps->add_sigma( -ps->delta_sigma() );
      }
      else if (ps->d_sigma() < 0.0) {
	ps->add_sigma( ps->delta_sigma() );
      }
      ps->d_old_sigma() = ps->d_sigma();
      ps->d_sigma() = 0.0;
    }
    else { // / if ((ps->d_sigma()*ps->d_old_sigma()) < 0.0)
      if (ps->d_old_sigma() > 0.0) {
	ps->add_sigma( ps->delta_sigma() );
      }
      else if (ps->d_old_sigma() < 0.0) {
	ps->add_sigma( -ps->delta_sigma() );
      }
      ps->mult_delta_sigma(eta_minus);
      ps->d_old_sigma() = 0.0;
      ps->d_sigma() = 0.0;
    }
    ++ps;
  } // /// end for all fsets 
  return sqrt(sum_error);
}

// //////////////////////////////////////////////////////////////////////

void 
FModel::RPROP_init(void) {
  // / 1. initialize consequences
  FRuleContainer::const_iterator pr = frules_.begin();
  while (pr != frules_.end()) { // /// for all rules
    Consequence::iterator pd_cons = pr->d_cons().begin();
    Consequence::iterator pd_old_cons = pr->d_old_cons().begin();
    Consequence::iterator p_delta = pr->delta_cons().begin();
    while (pd_cons != pr->d_cons().end()) {
      *(pd_cons++) = 0.0;
      *(pd_old_cons++) = 0.0;
      *(p_delta) = delta_cons_0;
    }
    ++pr;
  } // /// end all rules
  // / 2. initialize fsets
  FSetContainer::iterator ps = fsets_.begin();
  while (ps != fsets_.end()) { // /// for all fsets
    ps->d_mu() = 0.0;
    ps->d_old_mu() = 0.0;
    ps->set_delta_mu(delta_mu_0);
    ps->d_sigma() = 0.0;
    ps->d_old_sigma() = 0.0;
    ps->set_delta_sigma(delta_sigma_0);
    ++ps;
  } // /// end for all fsets 
  return;
}

// //////////////////////////////////////////////////////////////////////

void 
FModel::RPROP_backstep(void) {
#ifdef TRAPEZOIDAL_FSETS
  return;
#endif
  // / 1. take back the consequence parameters' update
  FRuleContainer::const_iterator pr = frules_.begin();
  while (pr != frules_.end()) { // /// for all rules
    Consequence::iterator pcons = pr->cons().begin();
    Consequence::iterator pd_old_cons = pr->d_old_cons().begin();
    Consequence::iterator p_delta = pr->delta_cons().begin();
    while (pcons != pr->cons().end()) {
      if ( *pd_old_cons > 0.0 ) {
	*pcons += *p_delta;
      }
      else if ( *pd_old_cons < 0.0 ) {
	*pcons -= *p_delta;
      }
      ++pcons;
      ++pd_old_cons;
      ++p_delta;
    }
    ++pr;
  } // /// end all rules
  // / 2. take back the fset parameters' update
  if (! GLOBAL::update_premise) {
    return;
  }
  FSetContainer::iterator ps = fsets_.begin();
  while (ps != fsets_.end()) { // /// for all fsets
    if (ps->d_old_mu() > 0.0) {
      ps->add_mu( ps->delta_mu() );
    }
    else if (ps->d_old_mu() < 0.0) {
      ps->add_mu( -ps->delta_mu() );
    }
    if (ps->d_old_sigma() > 0.0) {
      ps->add_sigma( ps->delta_sigma() );
    }
    else if (ps->d_old_sigma() < 0.0) {
      ps->add_sigma( -ps->delta_sigma() );
    }
    ++ps;
  } // /// end for all fsets 
  return;
}

// //////////////////////////////////////////////////////////////////////

std::ostream& operator << (std::ostream& strm, const FModel& fmodel)
{
  FRuleContainer::const_iterator pp;
  int original_precision = strm.precision();
  strm.precision(REAL_DIG);
  strm << "rdim = " << fmodel.rdim_ << endl;
  strm << "udim = " << fmodel.udim_ << endl;
  strm << "cdim = " << fmodel.cdim_ << endl;
  strm << "sdim = " << fmodel.sdim_ << endl;
  strm << "worst_rule = " << fmodel.worst_rule_ << endl;
  strm << "history = ";
  if (fmodel.rdim_ == 1) {
    strm << "(null)" << endl;
  }
  else {
    strm << fmodel.history_ << endl;
  }
  strm << "learnfilename = \"" << fmodel.learnfilename_ << "\"\n";
  strm << "validationfilename = \"" << fmodel.validationfilename_ << "\"\n";
  strm << "fsets:" << endl;
  if (fmodel.sdim_ == 0) {
    strm << "(null)" << endl;
  }
  else {
    strm << fmodel.fsets_;
  }
  strm << "consequences:" << endl;
  for (pp=fmodel.frules_.begin(); pp!=fmodel.frules_.end(); ++pp) {
    strm << "  C" << 1+(pp - fmodel.frules_.begin()) << ": ";
    strm << pp->cons() << pp->d_cons() << pp->d_old_cons();
    strm << pp->delta_cons() << endl;
  }
  // for all rules
  strm << "frules:" << endl;
  for (pp=fmodel.frules_.begin(); pp!=fmodel.frules_.end(); ++pp) {
    strm << "  R" << 1+(pp - fmodel.frules_.begin()) << ": if ";
    int n_clauses_printed = 0;
    Premise::const_iterator p = pp->prem().begin(); 
    //FSet** p = pp->prem().begin(); 
    while (p != pp->prem().end()) {
      if (*p != NULL) {
	if (n_clauses_printed > 0) {
	  strm << "and ";
	}
	strm << "U" << 1+((p - pp->prem().begin())/2) << " is F";
	//#WIN2017 strm << 1 + (*p - fmodel.fsets_.begin()) << " [";
        strm << 1 + (*p - &*(fmodel.fsets_.begin())) << " [";
	strm << (*p)->mu() << "," << (*p)->sigma() << "] ";
	++n_clauses_printed;
      }
      ++p;
    }
    if (n_clauses_printed == 0) {
      strm << "TRUE ";
    }
    strm << "then " << pp->cons() << endl;
  }
  // /// save denormalized model as comment
  const Data* pd = fmodel.learn_data();
  if (pd != 0) {
    vector<Real>::const_iterator sfactor = pd->scale_factor().begin();
    vector<Real>::const_iterator sshift = pd->scale_shift().begin();
    // for all rules
    strm << "#frules (original):" << endl;
    for (pp=fmodel.frules_.begin(); pp!=fmodel.frules_.end(); ++pp) {
      strm << "#  R" << 1+(pp - fmodel.frules_.begin()) << ": if ";
      int n_clauses_printed = 0;
      Premise::const_iterator p = pp->prem().begin(); 
      //FSet** p = pp->prem().begin(); 
      while (p != pp->prem().end()) {
	if (*p != NULL) {
	  if (n_clauses_printed > 0) {
	    strm << "and ";
	  }
	  int udim = ((p - pp->prem().begin())/2);
	  strm << "U" << 1+udim << " is F"
	       //#WIN2017 << 1 + (*p - fmodel.fsets_.begin()) << " ["
	       << 1 + (*p - &*(fmodel.fsets_.begin())) << " ["
	       << ( ((*p)->mu())/(*(sfactor+udim)) + *(sshift+udim) ) 
	       << ","
	       << ( (*p)->sigma() * *(sfactor+udim) )
	       << "] ";
	  ++n_clauses_printed;
	}
	++p;
      }
      if (n_clauses_printed == 0) {
	strm << "TRUE ";
      }
      strm << "then ";
      vector<Real>::const_iterator sfactor_y = pd->scale_factor().end() - 1;
      vector<Real>::const_iterator sshift_y = pd->scale_shift().end() - 1;
      int udim = 0;
      Real sum = *(pp->cons().begin());
      Consequence::const_iterator pc = pp->cons().begin() + 1;
      while (pc != pp->cons().end()) {
	sum -= *pc * *(sshift+udim) * *(sfactor+udim);
	++pc;
	++udim;
      }
      sum /= *sfactor_y;
      sum += *sshift_y;
      strm << sum;
      udim = 0;
      pc = pp->cons().begin() + 1;
      while (pc != pp->cons().end()) {
	strm << " " << *pc * *(sfactor+udim) / *sfactor_y;
	++pc;
	++udim;
      }
      strm << endl;
    }
  }
  else {
    strm << "#no denormalization info (scale factor and shift) available\n";
  }
  // /// ok, return
  strm.precision(original_precision);
  return strm;
}


/*
 * **********************************************************************
 * ********** miscellaneous functions
 * **********************************************************************
 */ 


#ifdef EXACT_SIGMOID
Real 
sigmoid(Real x)
{ 
  /// exact sigmoid
  return 1.0 / (1.0 + exp(x)); 
}
#else
Real 
sigmoid(Real x)
{ 
  static Real sigm_tab[] = {
    0.500000, 0.499125, 0.498250, 0.497375, 0.496500, 0.495625, 0.494750,
    0.493875, 0.493000, 0.492126, 0.491251, 0.490376, 0.489502, 0.488627,
    0.487752, 0.486878, 0.486004, 0.485129, 0.484255, 0.483381, 0.482507,
    0.481633, 0.480760, 0.479886, 0.479012, 0.478139, 0.477266, 0.476393,
    0.475520, 0.474647, 0.473774, 0.472902, 0.472029, 0.471157, 0.470285,
    0.469413, 0.468542, 0.467670, 0.466799, 0.465928, 0.465057, 0.464186,
    0.463316, 0.462446, 0.461576, 0.460706, 0.459837, 0.458967, 0.458099,
    0.457230, 0.456361, 0.455493, 0.454625, 0.453758, 0.452890, 0.452023,
    0.451156, 0.450290, 0.449424, 0.448558, 0.447692, 0.446827, 0.445962,
    0.445097, 0.444233, 0.443369, 0.442505, 0.441642, 0.440779, 0.439917,
    0.439055, 0.438193, 0.437331, 0.436470, 0.435610, 0.434749, 0.433889,
    0.433030, 0.432171, 0.431312, 0.430454, 0.429596, 0.428738, 0.427881,
    0.427025, 0.426169, 0.425313, 0.424458, 0.423603, 0.422749, 0.421895,
    0.421041, 0.420188, 0.419336, 0.418484, 0.417632, 0.416781, 0.415931,
    0.415081, 0.414231, 0.413382, 0.412534, 0.411686, 0.410839, 0.409992,
    0.409145, 0.408299, 0.407454, 0.406609, 0.405765, 0.404922, 0.404078,
    0.403236, 0.402394, 0.401553, 0.400712, 0.399872, 0.399032, 0.398193,
    0.397355, 0.396517, 0.395680, 0.394843, 0.394007, 0.393172, 0.392337,
    0.391503, 0.390669, 0.389836, 0.389004, 0.388173, 0.387342, 0.386511,
    0.385682, 0.384853, 0.384025, 0.383197, 0.382370, 0.381544, 0.380718,
    0.379894, 0.379069, 0.378246, 0.377423, 0.376601, 0.375780, 0.374959,
    0.374139, 0.373320, 0.372502, 0.371684, 0.370867, 0.370051, 0.369235,
    0.368420, 0.367606, 0.366793, 0.365980, 0.365169, 0.364358, 0.363547,
    0.362738, 0.361929, 0.361121, 0.360314, 0.359508, 0.358703, 0.357898,
    0.357094, 0.356291, 0.355488, 0.354687, 0.353886, 0.353086, 0.352287,
    0.351489, 0.350692, 0.349895, 0.349099, 0.348305, 0.347511, 0.346717,
    0.345925, 0.345134, 0.344343, 0.343553, 0.342764, 0.341976, 0.341189,
    0.340403, 0.339617, 0.338833, 0.338049, 0.337266, 0.336485, 0.335704,
    0.334923, 0.334144, 0.333366, 0.332589, 0.331812, 0.331037, 0.330262,
    0.329488, 0.328716, 0.327944, 0.327173, 0.326403, 0.325634, 0.324866,
    0.324098, 0.323332, 0.322567, 0.321803, 0.321039, 0.320277, 0.319515,
    0.318755, 0.317995, 0.317237, 0.316479, 0.315722, 0.314967, 0.314212,
    0.313458, 0.312706, 0.311954, 0.311203, 0.310454, 0.309705, 0.308957,
    0.308210, 0.307464, 0.306720, 0.305976, 0.305233, 0.304492, 0.303751,
    0.303011, 0.302272, 0.301535, 0.300798, 0.300063, 0.299328, 0.298594,
    0.297862, 0.297130, 0.296400, 0.295671, 0.294942, 0.294215, 0.293489,
    0.292764, 0.292039, 0.291316, 0.290594, 0.289873, 0.289153, 0.288434,
    0.287717, 0.287000, 0.286284, 0.285570, 0.284856, 0.284144, 0.283432,
    0.282722, 0.282013, 0.281305, 0.280597, 0.279891, 0.279187, 0.278483,
    0.277780, 0.277078, 0.276378, 0.275679, 0.274980, 0.274283, 0.273587,
    0.272892, 0.272198, 0.271505, 0.270813, 0.270123, 0.269433, 0.268745,
    0.268058, 0.267371, 0.266686, 0.266002, 0.265320, 0.264638, 0.263957,
    0.263278, 0.262600, 0.261923, 0.261246, 0.260572, 0.259898, 0.259225,
    0.258554, 0.257883, 0.257214, 0.256546, 0.255879, 0.255213, 0.254548,
    0.253885, 0.253222, 0.252561, 0.251901, 0.251242, 0.250584, 0.249927,
    0.249272, 0.248617, 0.247964, 0.247312, 0.246661, 0.246011, 0.245363,
    0.244715, 0.244069, 0.243424, 0.242780, 0.242137, 0.241495, 0.240855,
    0.240215, 0.239577, 0.238940, 0.238304, 0.237669, 0.237036, 0.236403,
    0.235772, 0.235142, 0.234513, 0.233885, 0.233259, 0.232634, 0.232009,
    0.231386, 0.230764, 0.230144, 0.229524, 0.228906, 0.228289, 0.227673,
    0.227058, 0.226444, 0.225832, 0.225220, 0.224610, 0.224001, 0.223393,
    0.222787, 0.222181, 0.221577, 0.220974, 0.220372, 0.219771, 0.219172,
    0.218573, 0.217976, 0.217380, 0.216785, 0.216192, 0.215599, 0.215008,
    0.214418, 0.213829, 0.213241, 0.212654, 0.212069, 0.211485, 0.210901,
    0.210320, 0.209739, 0.209159, 0.208581, 0.208004, 0.207428, 0.206853,
    0.206279, 0.205707, 0.205136, 0.204566, 0.203997, 0.203429, 0.202862,
    0.202297, 0.201733, 0.201170, 0.200608, 0.200047, 0.199488, 0.198929,
    0.198372, 0.197816, 0.197261, 0.196708, 0.196155, 0.195604, 0.195054,
    0.194505, 0.193957, 0.193411, 0.192865, 0.192321, 0.191778, 0.191236,
    0.190695, 0.190156, 0.189617, 0.189080, 0.188544, 0.188009, 0.187475,
    0.186943, 0.186411, 0.185881, 0.185352, 0.184824, 0.184297, 0.183772,
    0.183247, 0.182724, 0.182202, 0.181681, 0.181161, 0.180643, 0.180125,
    0.179609, 0.179094, 0.178580, 0.178067, 0.177555, 0.177045, 0.176535,
    0.176027, 0.175520, 0.175014, 0.174509, 0.174006, 0.173503, 0.173002,
    0.172502, 0.172003, 0.171505, 0.171008, 0.170512, 0.170018, 0.169525,
    0.169032, 0.168541, 0.168052, 0.167563, 0.167075, 0.166589, 0.166103,
    0.165619, 0.165136, 0.164654, 0.164173, 0.163693, 0.163215, 0.162737,
    0.162261, 0.161786, 0.161312, 0.160839, 0.160367, 0.159896, 0.159427,
    0.158958, 0.158491, 0.158025, 0.157559, 0.157095, 0.156633, 0.156171,
    0.155710, 0.155251, 0.154792, 0.154335, 0.153878, 0.153423, 0.152969,
    0.152516, 0.152064, 0.151614, 0.151164, 0.150716, 0.150268, 0.149822,
    0.149376, 0.148932, 0.148489, 0.148047, 0.147606, 0.147166, 0.146728,
    0.146290, 0.145854, 0.145418, 0.144984, 0.144550, 0.144118, 0.143687,
    0.143257, 0.142828, 0.142400, 0.141973, 0.141547, 0.141122, 0.140699,
    0.140276, 0.139854, 0.139434, 0.139014, 0.138596, 0.138179, 0.137762,
    0.137347, 0.136933, 0.136520, 0.136108, 0.135697, 0.135287, 0.134878,
    0.134470, 0.134063, 0.133657, 0.133253, 0.132849, 0.132446, 0.132045,
    0.131644, 0.131244, 0.130846, 0.130448, 0.130052, 0.129656, 0.129262,
    0.128869, 0.128476, 0.128085, 0.127694, 0.127305, 0.126917, 0.126529,
    0.126143, 0.125758, 0.125373, 0.124990, 0.124608, 0.124227, 0.123846,
    0.123467, 0.123089, 0.122711, 0.122335, 0.121960, 0.121586, 0.121212,
    0.120840, 0.120469, 0.120098, 0.119729, 0.119360, 0.118993, 0.118627,
    0.118261, 0.117897, 0.117533, 0.117171, 0.116809, 0.116449, 0.116089,
    0.115730, 0.115373, 0.115016, 0.114660, 0.114305, 0.113951, 0.113598,
    0.113247, 0.112896, 0.112545, 0.112196, 0.111848, 0.111501, 0.111155,
    0.110809, 0.110465, 0.110122, 0.109779, 0.109437, 0.109097, 0.108757,
    0.108418, 0.108080, 0.107744, 0.107408, 0.107072, 0.106738, 0.106405,
    0.106073, 0.105741, 0.105411, 0.105081, 0.104752, 0.104425, 0.104098,
    0.103772, 0.103447, 0.103123, 0.102799, 0.102477, 0.102156, 0.101835,
    0.101515, 0.101197, 0.100879, 0.100562, 0.100245, 0.099930, 0.099616,
    0.099302, 0.098990, 0.098678, 0.098367, 0.098057, 0.097748, 0.097440,
    0.097132, 0.096826, 0.096520, 0.096216, 0.095912, 0.095609, 0.095306,
    0.095005, 0.094704, 0.094405, 0.094106, 0.093808, 0.093511, 0.093215,
    0.092919, 0.092625, 0.092331, 0.092038, 0.091746, 0.091455, 0.091164,
    0.090875, 0.090586, 0.090298, 0.090011, 0.089725, 0.089439, 0.089155,
    0.088871, 0.088588, 0.088306, 0.088024, 0.087744, 0.087464, 0.087185,
    0.086907, 0.086630, 0.086353, 0.086077, 0.085802, 0.085528, 0.085255,
    0.084982, 0.084711, 0.084440, 0.084169, 0.083900, 0.083631, 0.083364,
    0.083096, 0.082830, 0.082565, 0.082300, 0.082036, 0.081773, 0.081510,
    0.081249, 0.080988, 0.080728, 0.080468, 0.080210, 0.079952, 0.079695,
    0.079439, 0.079183, 0.078928, 0.078674, 0.078421, 0.078168, 0.077916,
    0.077665, 0.077415, 0.077165, 0.076916, 0.076668, 0.076421, 0.076174,
    0.075928, 0.075683, 0.075439, 0.075195, 0.074952, 0.074710, 0.074468,
    0.074227, 0.073987, 0.073747, 0.073509, 0.073271, 0.073033, 0.072797,
    0.072561, 0.072326, 0.072091, 0.071858, 0.071624, 0.071392, 0.071160,
    0.070929, 0.070699, 0.070469, 0.070241, 0.070012, 0.069785, 0.069558,
    0.069332, 0.069106, 0.068881, 0.068657, 0.068434, 0.068211, 0.067989,
    0.067767, 0.067547, 0.067327, 0.067107, 0.066888, 0.066670, 0.066453,
    0.066236, 0.066020, 0.065804, 0.065590, 0.065375, 0.065162, 0.064949,
    0.064737, 0.064525, 0.064314, 0.064104, 0.063894, 0.063685, 0.063477,
    0.063269, 0.063062, 0.062855, 0.062650, 0.062444, 0.062240, 0.062036,
    0.061832, 0.061630, 0.061428, 0.061226, 0.061025, 0.060825, 0.060625,
    0.060426, 0.060228, 0.060030, 0.059833, 0.059636, 0.059440, 0.059245,
    0.059050, 0.058856, 0.058663, 0.058470, 0.058277, 0.058085, 0.057894,
    0.057704, 0.057514, 0.057324, 0.057135, 0.056947, 0.056759, 0.056572,
    0.056386, 0.056200, 0.056015, 0.055830, 0.055646, 0.055462, 0.055279,
    0.055096, 0.054914, 0.054733, 0.054552, 0.054372, 0.054192, 0.054013,
    0.053835, 0.053657, 0.053479, 0.053302, 0.053126, 0.052950, 0.052775,
    0.052600, 0.052426, 0.052253, 0.052079, 0.051907, 0.051735, 0.051564,
    0.051393, 0.051222, 0.051052, 0.050883, 0.050714, 0.050546, 0.050378,
    0.050211, 0.050045, 0.049878, 0.049713, 0.049548, 0.049383, 0.049219,
    0.049056, 0.048893, 0.048730, 0.048568, 0.048407, 0.048246, 0.048085,
    0.047925, 0.047766, 0.047607, 0.047448, 0.047291, 0.047133, 0.046976,
    0.046820, 0.046664, 0.046508, 0.046353, 0.046199, 0.046045, 0.045891,
    0.045738, 0.045586, 0.045434, 0.045282, 0.045131, 0.044981, 0.044831,
    0.044681, 0.044532, 0.044383, 0.044235, 0.044087, 0.043940, 0.043793,
    0.043647, 0.043501, 0.043355, 0.043210, 0.043066, 0.042922, 0.042778,
    0.042635, 0.042493, 0.042351, 0.042209, 0.042068, 0.041927, 0.041786,
    0.041646, 0.041507, 0.041368, 0.041229, 0.041091, 0.040954, 0.040816,
    0.040680, 0.040543, 0.040407, 0.040272, 0.040137, 0.040002, 0.039868,
    0.039734, 0.039601, 0.039468, 0.039335, 0.039203, 0.039072, 0.038941,
    0.038810, 0.038679, 0.038549, 0.038420, 0.038291, 0.038162, 0.038034,
    0.037906, 0.037779, 0.037652, 0.037525, 0.037399, 0.037273, 0.037148,
    0.037023, 0.036898, 0.036774, 0.036650, 0.036527, 0.036404, 0.036281,
    0.036159, 0.036037, 0.035916, 0.035795, 0.035674, 0.035554, 0.035434,
    0.035315, 0.035196, 0.035077, 0.034959, 0.034841, 0.034723, 0.034606,
    0.034490, 0.034373, 0.034257, 0.034142, 0.034026, 0.033912, 0.033797,
    0.033683, 0.033569, 0.033456, 0.033343, 0.033230, 0.033118, 0.033006,
    0.032895, 0.032783, 0.032673, 0.032562, 0.032452, 0.032342, 0.032233,
    0.032124, 0.032015, 0.031907, 0.031799, 0.031692, 0.031584, 0.031477,
    0.031371, 0.031265, 0.031159, 0.031053, 0.030948, 0.030844, 0.030739,
    0.030635, 0.030531, 0.030428, 0.030325, 0.030222, 0.030120, 0.030017,
    0.029916, 0.029814, 0.029713, 0.029612, 0.029512, 0.029412, 0.029312,
    0.029213, 0.029114, 0.029015, 0.028917, 0.028818, 0.028721, 0.028623,
    0.028526, 0.028429, 0.028333, 0.028236, 0.028141, 0.028045, 0.027950,
    0.027855, 0.027760, 0.027666, 0.027572, 0.027478, 0.027385, 0.027292,
    0.027199, 0.027107, 0.027014, 0.026923, 0.026831, 0.026740, 0.026649,
    0.026558, 0.026468, 0.026378, 0.026288, 0.026199, 0.026109, 0.026021,
    0.025932, 0.025844, 0.025756, 0.025668, 0.025581, 0.025494, 0.025407,
    0.025320, 0.025234, 0.025148, 0.025063, 0.024977, 0.024892, 0.024807,
    0.024723, 0.024638, 0.024554, 0.024471, 0.024387, 0.024304, 0.024221,
    0.024139, 0.024056, 0.023974, 0.023893, 0.023811, 0.023730, 0.023649,
    0.023568, 0.023488, 0.023408, 0.023328, 0.023248, 0.023169, 0.023090,
    0.023011, 0.022933, 0.022854, 0.022776, 0.022698, 0.022621, 0.022544,
    0.022467, 0.022390, 0.022313, 0.022237, 0.022161, 0.022086, 0.022010,
    0.021935, 0.021860, 0.021785, 0.021711, 0.021636, 0.021563, 0.021489,
    0.021415, 0.021342, 0.021269, 0.021196, 0.021124, 0.021052, 0.020980,
    0.020908, 0.020836, 0.020765, 0.020694, 0.020623, 0.020553, 0.020482,
    0.020412, 0.020342, 0.020273, 0.020203, 0.020134, 0.020065, 0.019996,
    0.019928, 0.019860, 0.019792, 0.019724, 0.019656, 0.019589, 0.019522,
    0.019455, 0.019388, 0.019322, 0.019256, 0.019190, 0.019124, 0.019058,
    0.018993, 0.018928, 0.018863, 0.018799, 0.018734, 0.018670, 0.018606,
    0.018542, 0.018478, 0.018415, 0.018352, 0.018289, 0.018226, 0.018164,
    0.018101, 0.018039, 0.017977, 0.017916, 0.017854, 0.017793, 0.017732,
    0.017671, 0.017610, 0.017550, 0.017490, 0.017430, 0.017370, 0.017310,
    0.017251, 0.017191, 0.017132, 0.017074, 0.017015, 0.016957, 0.016898,
    0.016840, 0.016782, 0.016725, 0.016667, 0.016610, 0.016553, 0.016496,
    0.016439, 0.016383, 0.016327, 0.016270, 0.016215, 0.016159, 0.016103,
    0.016048, 0.015993, 0.015938, 0.015883, 0.015828, 0.015774, 0.015720,
    0.015666, 0.015612, 0.015558, 0.015504, 0.015451, 0.015398, 0.015345,
    0.015292, 0.015240, 0.015187, 0.015135, 0.015083, 0.015031, 0.014979,
    0.014928, 0.014876, 0.014825, 0.014774, 0.014723, 0.014672, 0.014622,
    0.014572, 0.014521, 0.014471, 0.014422, 0.014372, 0.014322, 0.014273,
    0.014224, 0.014175, 0.014126, 0.014077, 0.014029, 0.013981, 0.013933,
    0.013885, 0.013837, 0.013789, 0.013741, 0.013694, 0.013647, 0.013600,
    0.013553, 0.013506, 0.013460, 0.013413, 0.013367, 0.013321, 0.013275,
    0.013229, 0.013184, 0.013138, 0.013093, 0.013048, 0.013003, 0.012958,
    0.012913, 0.012869, 0.012824, 0.012780, 0.012736, 0.012692, 0.012648,
    0.012605, 0.012561, 0.012518, 0.012475, 0.012432, 0.012389, 0.012346,
    0.012303, 0.012261, 0.012219, 0.012176, 0.012134, 0.012093, 0.012051,
    0.012009, 0.011968, 0.011926, 0.011885, 0.011844, 0.011803, 0.011763,
    0.011722, 0.011681, 0.011641, 0.011601, 0.011561, 0.011521, 0.011481,
    0.011442, 0.011402, 0.011363, 0.011323, 0.011284, 0.011245, 0.011206,
    0.011168, 0.011129, 0.011091, 0.011052, 0.011014, 0.010976, 0.010938,
    0.010900, 0.010863, 0.010825, 0.010788, 0.010750, 0.010713, 0.010676,
    0.010639, 0.010603, 0.010566, 0.010529, 0.010493, 0.010457, 0.010421,
    0.010385, 0.010349, 0.010313, 0.010277, 0.010242, 0.010206, 0.010171,
    0.010136, 0.010101, 0.010066, 0.010031, 0.009996, 0.009962, 0.009927,
    0.009893, 0.009859, 0.009825, 0.009791, 0.009757, 0.009723, 0.009689,
    0.009656, 0.009622, 0.009589, 0.009556, 0.009523, 0.009490, 0.009457,
    0.009424, 0.009392, 0.009359, 0.009327, 0.009294, 0.009262, 0.009230,
    0.009198, 0.009166, 0.009135, 0.009103, 0.009072, 0.009040, 0.009009,
    0.008978, 0.008947, 0.008916, 0.008885, 0.008854, 0.008823, 0.008793,
    0.008762, 0.008732, 0.008702, 0.008672, 0.008642, 0.008612, 0.008582,
    0.008552, 0.008522, 0.008493, 0.008463, 0.008434, 0.008405, 0.008376,
    0.008347, 0.008318, 0.008289, 0.008260, 0.008232, 0.008203, 0.008175,
    0.008146, 0.008118, 0.008090, 0.008062, 0.008034, 0.008006, 0.007978,
    0.007951, 0.007923, 0.007896, 0.007868, 0.007841, 0.007814, 0.007787,
    0.007760, 0.007733, 0.007706, 0.007679, 0.007653, 0.007626, 0.007600,
    0.007573, 0.007547, 0.007521, 0.007495, 0.007469, 0.007443, 0.007417,
    0.007392, 0.007366, 0.007340, 0.007315, 0.007290, 0.007264, 0.007239,
    0.007214, 0.007189, 0.007164, 0.007139, 0.007114, 0.007090, 0.007065,
    0.007041, 0.007016, 0.006992, 0.006968, 0.006943, 0.006919, 0.006895,
    0.006871, 0.006848, 0.006824, 0.006800, 0.006776, 0.006753, 0.006730,
    0.006706, 0.006683, 0.006660, 0.006637, 0.006614, 0.006591, 0.006568,
    0.006545, 0.006522, 0.006500, 0.006477, 0.006455, 0.006432, 0.006410,
    0.006388, 0.006365, 0.006343, 0.006321, 0.006299, 0.006277, 0.006256,
    0.006234, 0.006212, 0.006191, 0.006169, 0.006148, 0.006126, 0.006105,
    0.006084, 0.006063, 0.006042, 0.006021, 0.006000, 0.005979, 0.005958,
    0.005938, 0.005917, 0.005896, 0.005876, 0.005856, 0.005835, 0.005815,
    0.005795, 0.005775, 0.005755, 0.005735, 0.005715, 0.005695, 0.005675,
    0.005655, 0.005636, 0.005616, 0.005597, 0.005577, 0.005558, 0.005538,
    0.005519, 0.005500, 0.005481, 0.005462, 0.005443, 0.005424, 0.005405,
    0.005386, 0.005368, 0.005349, 0.005330, 0.005312, 0.005293, 0.005275,
    0.005257, 0.005238, 0.005220, 0.005202, 0.005184, 0.005166, 0.005148,
    0.005130, 0.005112, 0.005094, 0.005077, 0.005059, 0.005041, 0.005024,
    0.005006, 0.004989, 0.004972, 0.004954, 0.004937, 0.004920, 0.004903,
    0.004886, 0.004869, 0.004852, 0.004835, 0.004818, 0.004802, 0.004785,
    0.004768, 0.004752, 0.004735, 0.004719, 0.004702, 0.004686, 0.004670,
    0.004653, 0.004637, 0.004621, 0.004605, 0.004589, 0.004573, 0.004557,
    0.004541, 0.004525, 0.004510, 0.004494, 0.004478, 0.004463, 0.004447,
    0.004432, 0.004416, 0.004401, 0.004386, 0.004370, 0.004355, 0.004340,
    0.004325, 0.004310, 0.004295, 0.004280, 0.004265, 0.004250, 0.004236,
    0.004221, 0.004206, 0.004191, 0.004177, 0.004162, 0.004148, 0.004133,
    0.004119, 0.004105, 0.004090, 0.004076, 0.004062, 0.004048, 0.004034,
    0.004020, 0.004006, 0.003992, 0.003978, 0.003964, 0.003950, 0.003937,
    0.003923, 0.003909, 0.003896, 0.003882, 0.003869, 0.003855, 0.003842,
    0.003828, 0.003815, 0.003802, 0.003788, 0.003775, 0.003762, 0.003749,
    0.003736, 0.003723, 0.003710, 0.003697, 0.003684, 0.003671, 0.003659,
    0.003646, 0.003633, 0.003621, 0.003608, 0.003595, 0.003583, 0.003570,
    0.003558, 0.003546, 0.003533, 0.003521, 0.003509, 0.003496, 0.003484,
    0.003472, 0.003460, 0.003448, 0.003436, 0.003424, 0.003412, 0.003400,
    0.003388, 0.003377, 0.003365, 0.003353, 0.003341, 0.003330, 0.003318,
    0.003307, 0.003295, 0.003284, 0.003272, 0.003261, 0.003249, 0.003238,
    0.003227, 0.003216, 0.003204, 0.003193, 0.003182, 0.003171, 0.003160,
    0.003149, 0.003138, 0.003127, 0.003116, 0.003105, 0.003095, 0.003084,
    0.003073, 0.003062, 0.003052, 0.003041, 0.003030, 0.003020, 0.003009,
    0.002999, 0.002988, 0.002978, 0.002968, 0.002957, 0.002947, 0.002937,
    0.002927, 0.002916, 0.002906, 0.002896, 0.002886, 0.002876, 0.002866,
    0.002856, 0.002846, 0.002836, 0.002826, 0.002816, 0.002807, 0.002797,
    0.002787, 0.002777, 0.002768, 0.002758, 0.002748, 0.002739, 0.002729,
    0.002720, 0.002710, 0.002701, 0.002691, 0.002682, 0.002673, 0.002663,
    0.002654, 0.002645, 0.002636, 0.002626, 0.002617, 0.002608, 0.002599,
    0.002590, 0.002581, 0.002572, 0.002563, 0.002554, 0.002545, 0.002536,
    0.002527, 0.002519, 0.002510, 0.002501, 0.002492, 0.002484, 0.002475,
    0.002466, 0.002458, 0.002449, 0.002441, 0.002432, 0.002424, 0.002415,
    0.002407, 0.002399, 0.002390, 0.002382, 0.002374, 0.002365, 0.002357,
    0.002349, 0.002341, 0.002332, 0.002324, 0.002316, 0.002308, 0.002300,
    0.002292, 0.002284, 0.002276, 0.002268, 0.002260, 0.002252, 0.002245,
    0.002237, 0.002229, 0.002221, 0.002213, 0.002206, 0.002198, 0.002190,
    0.002183, 0.002175, 0.002168, 0.002160, 0.002152, 0.002145, 0.002137,
    0.002130, 0.002123, 0.002115, 0.002108, 0.002100, 0.002093, 0.002086,
    0.002079, 0.002071, 0.002064, 0.002057, 0.002050, 0.002043, 0.002035,
    0.002028, 0.002021, 0.002014, 0.002007, 0.002000, 0.001993, 0.001986,
    0.001979, 0.001972, 0.001966, 0.001959, 0.001952, 0.001945, 0.001938,
    0.001932, 0.001925, 0.001918, 0.001911, 0.001905, 0.001898, 0.001891,
    0.001885, 0.001878, 0.001872, 0.001865, 0.001859, 0.001852, 0.001846,
    0.001839, 0.001833, 0.001827, 0.001820, 0.001814, 0.001808, 0.001801,
    0.001795, 0.001789, 0.001782, 0.001776, 0.001770, 0.001764, 0.001758,
    0.001752, 0.001745, 0.001739, 0.001733, 0.001727, 0.001721, 0.001715,
    0.001709, 0.001703, 0.001697, 0.001691, 0.001686, 0.001680, 0.001674,
    0.001668, 0.001662, 0.001656, 0.001651, 0.001645, 0.001639, 0.001633,
    0.001628, 0.001622, 0.001616, 0.001611, 0.001605, 0.001599, 0.001594,
    0.001588, 0.001583, 0.001577, 0.001572, 0.001566, 0.001561, 0.001555,
    0.001550, 0.001545, 0.001539, 0.001534, 0.001528, 0.001523, 0.001518,
    0.001512, 0.001507, 0.001502, 0.001497, 0.001491, 0.001486, 0.001481,
    0.001476, 0.001471, 0.001466, 0.001461, 0.001455, 0.001450, 0.001445,
    0.001440, 0.001435, 0.001430, 0.001425, 0.001420, 0.001415, 0.001410,
    0.001405, 0.001401, 0.001396, 0.001391, 0.001386, 0.001381, 0.001376,
    0.001371, 0.001367, 0.001362, 0.001357, 0.001352, 0.001348, 0.001343,
    0.001338, 0.001334, 0.001329, 0.001324, 0.001320, 0.001315, 0.001311,
    0.001306, 0.001301, 0.001297, 0.001292, 0.001288, 0.001283, 0.001279,
    0.001274, 0.001270, 0.001266, 0.001261, 0.001257, 0.001252, 0.001248,
    0.001244, 0.001239, 0.001235, 0.001231, 0.001226, 0.001222, 0.001218,
    0.001214, 0.001209, 0.001205, 0.001201, 0.001197, 0.001193, 0.001188,
    0.001184, 0.001180, 0.001176, 0.001172, 0.001168, 0.001164, 0.001160,
    0.001156, 0.001152, 0.001148, 0.001144, 0.001140, 0.001136, 0.001132,
    0.001128, 0.001124, 0.001120, 0.001116, 0.001112, 0.001108, 0.001104,
    0.001100, 0.001097, 0.001093, 0.001089, 0.001085, 0.001081, 0.001078,
    0.001074, 0.001070, 0.001066, 0.001063, 0.001059, 0.001055, 0.001051,
    0.001048, 0.001044, 0.001041, 0.001037, 0.001033, 0.001030, 0.001026,
    0.001022, 0.001019, 0.001015, 0.001012, 0.001008, 0.001005, 0.001001,
    0.000998, 0.000994, 0.000991, 0.000987, 0.000984, 0.000980, 0.000977,
    0.000974, 0.000970, 0.000967, 0.000963, 0.000960, 0.000957, 0.000953,
    0.000950, 0.000947, 0.000943, 0.000940, 0.000937, 0.000934, 0.000930,
    0.000927, 0.000924, 0.000921, 0.000917, 0.000914, 0.000911, 0.000908 
  };
  const Real real_min = 0.00001;
  // const Real real_min = 16 * REAL_MIN;
  // const Real real_min = 0.0;
  if (x < 0.0) { 
    if (x < -7.0) {
      return 1.0 - real_min;
    }
    else {
      /// index: |_ x * 2000/7 _|
      return 1.0 - sigm_tab[ ((unsigned) (-x * 285.71)) ];
    }
  }
  else {
    if (x > 7) {
      return real_min;
    }
    else {
      return sigm_tab[ ((unsigned) (x * 285.71)) ];
    }
  }
} /// end Real sigmoid(Real x)
#endif /// #ifdef EXACT_SIGMOID

