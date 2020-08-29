#ifndef FZYESTIM_HH
#define FZYESTIM_HH

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
 * $Id: fzyestim.hh,v 2.9 2020-08-29 10:47:37+00:00:02 manfred Exp $
 */


#include "fmodel.hh"


/** Write the one-step (estimation) or multiple step prediction (simulation)
 * of a fuzzy model for a given data set
 * 
 * The output file names are created using the input file name
 * as prefix:
 * Writes estimation results to est_[filename].out
 * or simulation results to sim_[filename].out,
 * R2 to est_[filename_extension].r2 or sim_[filename_extension].r2,
 * and error to est_[filename].err or sim_[filename].err.
 * @memo
 */
void fzyestim(char* fzyfilename, char* datafilename) throw (Error);


#endif /* #ifndef FZYESTIM_HH */
