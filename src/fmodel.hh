#ifndef FMODEL_HH
#define FMODEL_HH

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
 * $Id: fmodel.hh,v 2.9 2020-08-29 10:47:36+00:00:02 manfred Exp $
 */


#ifndef WIN2017
#include <vector.h>  // STL vectors
#include <vec.h>     // TNT vectors
#else
#include <vector>    // STL vectors
#include <tnt.h>     // TNT vectors
#endif

#include "global.hh"
#include "data.hh"
#include "param.hh"
#include "funct.hh"


/** @name Fuzzy Set funtions
 */
//@{
/** Membership value of trapezoidal fuzzy set.
 *
 * @memo
 * @param sigma steepness
 * @param u position
 * @return membership value
 */
inline Real trapezoid(Real sigma, Real u) {
  Real y = 0.5 + sigma * u;
  if (y < 0.0) {
    return 0.0;
  }
  if (y > 1.0) {
    return 1.0;
  }
  return y;
}

/** Membership value of sigmoidal fuzzy set.
 *
 * @memo
 * @param x position
 * @return membership value
 */
Real sigmoid(Real x);

/** Product t-norm.
 * @memo
 */
inline Real t_norm(Real a, Real b) { 
  return (a * b);
}

/** Product inference.
 * @memo
 */
inline Real inference(Real prem, Real cons) {
  return (prem * cons);
}
//@}



/*
 * **********************************************************************
 * ********** FSet
 * **********************************************************************
 */

/** Fuzzy Set.
 * @memo
 */
class FSet 
{
protected:
  Real mu_;
  Real sigma_;
  Real d_mu_;
  Real d_sigma_;
  Real d_old_mu_;
  Real d_old_sigma_;
  Real delta_mu_;
  Real delta_sigma_;
  /// check border of $\Delta\mu$
  inline void limit_delta_mu(void) {
    if (delta_mu_ > max_delta_mu)
      delta_mu_ = max_delta_mu;
    if (delta_mu_ < min_delta_mu)
      delta_mu_ = min_delta_mu;
  }
  /// check border of $\Delta\sigma$
  inline void limit_delta_sigma(void) {
    if (delta_sigma_ > max_delta_sigma)
      delta_sigma_ = max_delta_sigma;
    if (delta_sigma_ < min_delta_sigma)
      delta_sigma_ = min_delta_sigma;
  }
public:
  FSet()
    : mu_(0.5*(mu_left+mu_right)), sigma_(sigma_0), d_mu_(0.0), d_sigma_(0.0),
      d_old_mu_(0.0), d_old_sigma_(0.0), 
      delta_mu_(delta_mu_0), delta_sigma_(delta_sigma_0) { }
  FSet(Real m, Real s)
    : mu_(m), sigma_(s), d_mu_(0.0), d_sigma_(0.0), d_old_mu_(0.0),
      d_old_sigma_(0.0), delta_mu_(delta_mu_0), delta_sigma_(delta_sigma_0) { }
  const Real& mu() const { return mu_; }
  const Real& sigma() const { return sigma_; }
  const Real& d_mu() const { return d_mu_; }
  const Real& d_sigma() const { return d_sigma_; }
  const Real& d_old_mu() const { return d_old_mu_; }
  const Real& d_old_sigma() const { return d_old_sigma_; }
  const Real& delta_mu() const { return delta_mu_; }
  const Real& delta_sigma() const { return delta_sigma_; }
  Real& mu() { return mu_; }
  Real& sigma() { return sigma_; }
  Real& d_mu() { return d_mu_; }
  Real& d_sigma() { return d_sigma_; }
  Real& d_old_mu() { return d_old_mu_; }
  Real& d_old_sigma() { return d_old_sigma_; }
  Real& delta_mu() { return delta_mu_; }
  Real& delta_sigma() { return delta_sigma_; }
  void set_mu(Real m) { mu_ = m; }
  void set_sigma(Real s){     
    //    sigma_ = s ; 
    sigma_ = s;
    if (sigma_ > 0.0) {
      if (sigma_ > GLOBAL::max_sigma) sigma_ = GLOBAL::max_sigma;
      if (sigma_ < GLOBAL::min_sigma) sigma_ = GLOBAL::min_sigma;
    }
    else if (sigma_ < 0.0) { 
      if (sigma_ < -GLOBAL::max_sigma) sigma_ = -GLOBAL::max_sigma;
      if (sigma_ > -GLOBAL::min_sigma) sigma_ = -GLOBAL::min_sigma;
    }
  }
  void add_mu(Real m) { mu_ += m; }
  void add_sigma(Real s) { 
    if (sigma_ > 0.0) {
      sigma_ += s;
      if (sigma_ > GLOBAL::max_sigma) sigma_ = GLOBAL::max_sigma;
      if (sigma_ < GLOBAL::min_sigma) sigma_ = GLOBAL::min_sigma;
    }
    else if (sigma_ < 0.0) { 
      sigma_ += s;
      if (sigma_ < -GLOBAL::max_sigma) sigma_ = -GLOBAL::max_sigma;
      if (sigma_ > -GLOBAL::min_sigma) sigma_ = -GLOBAL::min_sigma;
    }
    else {
      set_sigma(s);
    }
  }
  void add_d_mu(Real m) { d_mu_ += m; }
  void add_d_sigma(Real s) { d_sigma_ += s; }
  void set_delta_mu(Real dm) { delta_mu_ = dm; limit_delta_mu(); }
  void set_delta_sigma(Real ds) { delta_sigma_ = ds; limit_delta_sigma(); }
  void mult_delta_mu(Real eta) { delta_mu_ *= eta; limit_delta_mu(); }
  void mult_delta_sigma(Real eta) { delta_sigma_ *= eta; limit_delta_sigma(); }
  /// compute the membership value
  Real F(Real u) { 
#ifdef TRAPEZOIDAL_FSETS
    return trapezoid(sigma_, (u-mu_));
#else
    return sigmoid(sigma_ * (u-mu_)); 
#endif
  }
  /// print a fuzzy set to an output stream
  friend std::ostream& operator << (std::ostream& strm, const FSet& fset);
};

// print a fuzzy set vector to an output stream
std::ostream& operator << (std::ostream &strm, const vector<FSet> &v);



/*
 * **********************************************************************
 * ********** FRule
 * **********************************************************************
 */

/** Fuzzy Rule's Premise: a vector of pointers to Fuzzy Sets.
 * @type typedef
 * @memo
 */
typedef vector<FSet*> Premise;

/** Fuzzy Rule's Consequence: a vector of its parameters.
 * @type typedef
 * @memo
 */
typedef Vector<Real> Consequence;

/** Fuzzy Rule.
 * @memo
 */
class FRule
{
protected:
  Premise prem_;
  Consequence cons_;
  Consequence d_cons_;
  Consequence d_old_cons_;
  Consequence delta_cons_;
public:
  FRule() {  }
  FRule(size_t u_dimension, size_t cons_dimension);
  const Premise& prem() const { return prem_; }
  const Consequence& cons() const { return cons_; }
  const Consequence& d_cons() const { return d_cons_; }
  const Consequence& d_old_cons() const { return d_old_cons_; }
  const Consequence& delta_cons() const { return delta_cons_; }
  Premise& prem() { return prem_; }
  Consequence& cons() { return cons_; }
  Consequence& d_cons() { return d_cons_; }
  Consequence& d_old_cons() { return d_old_cons_; }
  Consequence& delta_cons() { return delta_cons_; }
  FSet*& prem(size_t index) { return prem_[index]; }
  Real& cons(Subscript index) { return cons_[index]; }
  Real& d_cons(Subscript index) { return d_cons_[index]; }
  Real& d_old_cons(Subscript index) { return d_old_cons_[index]; }
  Real& delta_cons(Subscript index) { return delta_cons_[index]; }
  /// copy a Fuzzy Rule by value
  void copy(const FRule& r, FSet* fset_begin, const FSet* r_fset_begin);
  /// compute a Fuzzy Rule's consequence value for a given input vector
  Real consvalue(const Uvector& u) const {
    assert(cons_.size() > 0);
    assert(cons_.size() <= u.size()+1);
    Consequence::const_iterator pcons = cons_.begin();
    Uvector::const_iterator pu = u.begin();
    assert(pcons != NULL);
    register Real consvalue = *(pcons++);
    while (pcons != cons_.end()) {
      assert(pu != u.end());
      consvalue += *(pcons++) * *(pu++);
    }
    return consvalue;
  }
  /// compute a Fuzzy Rule's premise value for a given input vector
  Real premvalue(const Uvector& u) const {
    assert(prem_.size() == 2U * u.size());
    assert(prem_.size() > 0);
    Premise::const_iterator pprem = prem_.begin();
    Uvector::const_iterator pu = u.begin();
    register Real premvalue = 1.0;
    assert(pprem != prem_.end());
    do {
      assert(pu != u.end());
      if (*pprem != NULL) 
	premvalue *= (*pprem++)->F(*pu);
      else 
	++pprem;
      assert(pprem != prem_.end());
      if (*pprem != NULL) 
	premvalue *= (*pprem++)->F(*pu);
      else 
	++pprem;
      ++pu;
    } while (pprem != prem_.end());
    return premvalue;
  }
  /// refine a Fuzzy Rule in uindex u; save additional new rule under FRule* r
  void refine(FRule* r, size_t u, FSet* newleftfset, FSet* newrightfset);
};




/*
 * **********************************************************************
 * ********** FModel
 * **********************************************************************
 */

/** Fuzzy Set Container: a vector of Fuzzy Sets
 * @type typedef
 * @memo
 */
typedef vector<FSet> FSetContainer;
/** Fuzzy Rule Container: a vector of Fuzzy Rules
 * @type typedef
 * @memo
 */
typedef vector<FRule> FRuleContainer;

/** Fuzzy Model.
 * @memo
 */
class FModel : public Function
{
protected:
  /// number of rules
  size_t rdim_; 
  /// input space dimension
  size_t udim_;
  /// consequence dimension 
  size_t cdim_;
  /// number of fsets = 2 * (rdim-1)
  size_t sdim_; 
  /// index of rule with biggest error
  size_t worst_rule_;
  /// history of refinements
  vector<size_t> history_;
  /// the model's Fuzzy Sets
  FSetContainer fsets_;
  /// the models Fuzzy Rules
  FRuleContainer frules_;
  /// name of the learning/training data file 
  string learnfilename_;
  /// name of the validation data file 
  string validationfilename_;
  /// for the usage of a Minimizer
  vector<ConsParam> cons_;
  vector<MuParam> mu_;
  vector<SigmaParam> sigma_;
  Data *learn_data_;
  Data *valid_data_;
  /// check border of $\Delta$ c
  inline void limit_delta_cons(Consequence::iterator& p) {
    if (*p > max_delta_cons)
      *p = max_delta_cons;
    if (*p < min_delta_cons)
      *p = min_delta_cons;
  }
public:
  /// empty model
  FModel() : rdim_(0), udim_(0), cdim_(0), sdim_(0), worst_rule_(0),
    learn_data_(0), valid_data_(0) { }
  /// initial one-rule model
  FModel(const Data& d, size_t cons_dimension) 
    : rdim_(1), udim_(d.udim()), cdim_(cons_dimension), sdim_(0),
      worst_rule_(0), learn_data_(0), valid_data_(0) {
    FRule r1(d.udim(), cons_dimension);
    frules_.resize(rdim_);
    frules_[0] = r1;
  }
  ~FModel() { }
  const size_t& rdim() const { return rdim_; }
  const size_t& udim() const { return udim_; }
  const size_t& cdim() const { return cdim_; }
  const size_t& sdim() const { return sdim_; }
  const size_t& worst_rule() const { return worst_rule_; }
  const vector<size_t>& history() const { return history_; }
  const FSet& fsets(size_t index) const { return fsets_[index]; }
  const FRule& frules(size_t index) const { return frules_[index]; }
  const string& learnfilename() const { return learnfilename_; }
  const string& validationfilename() const { return validationfilename_; }
  const Data* learn_data() const { return learn_data_; }
  const Data* valid_data() const { return valid_data_; }
  const vector<Parameter*>& parameters() const { return parameters_; }
  size_t& rdim() { return rdim_; }
  size_t& udim() { return udim_; }
  size_t& cdim() { return cdim_; }
  size_t& sdim() { return sdim_; }
  size_t& worst_rule() { return worst_rule_; }
  vector<size_t>& history() { return history_; }
  FSet& fsets(size_t index) { return fsets_[index]; }
  FRule& frules(size_t index) { return frules_[index]; }
  string& learnfilename() { return learnfilename_; }
  string& validationfilename() { return validationfilename_; }
  Data*& learn_data() { return learn_data_; }
  Data*& valid_data() { return valid_data_; }
  vector<Parameter*>& parameters() { return parameters_; }

  /// copy a whole model
  void copy(const FModel& oldmodel);
  /// refine a model's structure in rule rindex at index uindex
  FModel(const FModel& oldmodel, size_t ruleindex, size_t uindex);
  /// load a model from file
  void load(char* filename) throw (Error);

  /// w = sum(premise values)
  Real sum_w(const Uvector& u) {
    assert(frules_.size() > 0);
    FRuleContainer::const_iterator pfrule = frules_.begin();
    register Real sum_premvalues = 0.0;
    while (pfrule != frules_.end()) {
      sum_premvalues += (pfrule++)->premvalue(u);
    }
    return sum_premvalues;
  }
  /// feedforward step; returns $\hat{y}$
  Real y_hat(const Uvector& u) throw (Error);

  /// optimize consequence parameters using SVD
  void optimize_SVD(const Data& d) throw (Error);
  /// reset consequence parameters
  void reset_consequences(void);
  /// initialize model for RPROP
  void RPROP_init(void);
  /// one RPROP step; returns $\varepsilon = (y - \hat{y})^2$
  Real RPROP(const Data& learndata) throw (Error);
  /// RPROP backstep: cancel last RPROP parameter update
  void RPROP_backstep(void);
  /// optimize model by cross validation; returns last error
  Real optimize_RPROP(const Data& a, const Data& b, 
		      size_t min_iterations, size_t max_iterations);

  /// initialize model for GRAD_DESCENT
  void GRAD_DESCENT_init(void);
  /// one GRAD_DESCENT step; returns $\varepsilon = (y - \hat{y})^2$
  Real GRAD_DESCENT(const Data& learndata) throw (Error);
  /// GRAD_DESCENT backstep: cancel last GRAD_DESCENT parameter update
  void GRAD_DESCENT_backstep(void);
  /// optimize model by cross validation; returns last error
  Real optimize_GRAD_DESCENT(const Data& a, const Data& b,
			     size_t min_iterations, size_t max_iterations);

  /// estimation error; returns R2
  Real R2(const Data& d);
  /// determine index of rule with biggest approximation error
  size_t worst_rule_index(const Data& d) throw (Error);
  /// estimation error; returns $\varepsilon = (y - \hat{y})^2$
  Real estimation(const Data& d) throw (Error) {
    return estimation(d, NULL);
  }
  /// write estimation into a file; returns estimation error
  Real estimation(const Data& d, const char* outfilename) throw (Error);
  /// simulation error; returns simulation error
  Real simulation(const Data& d) throw (Error) {
    return simulation(d, NULL);
  }
  /// write simulation into a file; returns simulation error
  Real simulation(const Data& d, const char* outfilename) throw (Error);
  /// print the whole model to an output stream
  friend std::ostream& operator << (std::ostream& strm, const FModel& fmodel);

  /// prepare FModel for Minimizer
  void create_parameters(Data* learning_data, Data* validation_data);
  /// erase parameters for Minimizer
  void erase_parameters();
  /// calculate function value (for learning/training)
  Real calculate() {
    if (learn_data_ != 0) {
      if ( (GLOBAL::parallel_optimization == HOOKE_JEEVES)
	   || (GLOBAL::parallel_optimization == ROSENBROCK) ){
	//	return simulation(*learn_data_);
	Real sim_learn = simulation(*learn_data_);
	return sim_learn;
      }
      else {
	//	return estimation(*learn_data_);
	Real est_learn = estimation(*learn_data_);
	return est_learn;
      }
    }
    return REAL_MAX;
  }
  /// calculate function value (for validation)
  Real validate() {
    if (valid_data_ != 0) {
      if ( (GLOBAL::parallel_optimization == HOOKE_JEEVES)
	   || (GLOBAL::parallel_optimization == ROSENBROCK) ){
	//	return simulation(*valid_data_);
	Real sim_valid = simulation(*valid_data_);
	return sim_valid;
      }
      else {
	//	return estimation(*valid_data_);
	Real est_valid = estimation(*valid_data_);
	return est_valid;
      }
    }
    return REAL_MAX;
  }
  /// calculate derivation
  void derivate() { throw Error("derivate() not yet implemented!"); return; }
};


/// we only have one function to minimize
//typedef FModel Function;


#endif /* ifndef FMODEL_HH */
