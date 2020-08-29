#ifndef DATA_HH
#define DATA_HH

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
 * $Id: data.hh,v 2.9 2020-08-29 10:47:35+00:00:02 manfred Exp $
 */


/* includes:
 */

#ifndef WIN2017
#include <vector.h>  // STL vectors
#include <vec.h>     // TNT vectors
#else
#include <vector>    // STL vectors
#include <tnt.h>     // TNT vectors
#endif


#include "global.hh"



/* Stream output operator for vectors. 
 */
std::ostream& operator<<(std::ostream &strm, const vector<Real> &v);


/** Type of an input value vector u (u-part of one pattern).
 * @type typedef
 * @memo
 */
typedef Vector<Real> Uvector;

/* Stream output operator for Uvectors.  
 */
std::ostream& operator<<(std::ostream &strm, const Uvector &v);


/** Data container.
 * @memo
 */
class Data 
{
protected:
  string filename_;
  /// block size for twodimensional plots
  size_t blocksize_;
  /// u-parts of the patterns 
  vector<Uvector> U_;
  /// y-parts of the patterns 
  vector<Real> y_;
  /// mean of all y values
  Real mean_y_;
  /// variance of all y values
  Real variance_y_;
  /** scale factor
   * Holds scale(u) in scale_factor_[0...n_columns-1] and 
   * scale(y) in scale_factor_[n_columns].
   * @memo
   */
  vector<Real> scale_factor_; 
  /** scale shift
   * Holds shift(u) in shift_factor_[0...n_columns-1] and 
   * shift(y) in shift_factor_[n_columns].
   * @memo
   */
  vector<Real> scale_shift_;
public:
  /// empty data
  Data() : filename_("") { }
  /// create data matrix filled with x0
  Data(Subscript N, size_t M, Real x0);
  const size_t udim() const { 
    if (U_.size() > 0) 
      return (size_t)U_[0].size();
    else 
      return 0;
  }
  /// filename of loaded data
  const string& filename() const { return filename_; }
  /// u-parts of all patterns
  const vector<Uvector>& U() const { return U_; }
  /// y-parts of all patterns
  const vector<Real>& y() const { return y_; }
  /// scale factor
  const vector<Real>& scale_factor() const { return scale_factor_; }
  /// scale shift
  const vector<Real>& scale_shift() const { return scale_shift_; }
  /// blocksize (for twodimensional print)
  const size_t& blocksize() const { return blocksize_; }
  /// mean of all y values
  const Real& mean_y() const { return mean_y_; }
  /// variance of all y values
  const Real& variance_y() const { return variance_y_; }
  /// modify u-parts of all patterns
  vector<Uvector>& U() { return U_; }
  /// modify y-parts of all patterns
  vector<Real>& y() { return y_; }
  /// load data matrix from file
  void load(char* filename) throw (Error);
  /// normalize data matrix with given scale_factor and scale_shift
  void normalize(const vector<Real>& factor, const vector<Real>& shift) 
    throw (Error);
  /// print data into a stream
  friend std::ostream& operator << (std::ostream& strm, const Data& d);
};


#endif /// #ifndef DATA_HH
