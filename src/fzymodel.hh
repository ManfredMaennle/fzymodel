#ifndef FZYMODEL_HH
#define FZYMODEL_HH

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
 * $Id: fzymodel.hh,v 2.9 2020-08-29 10:47:38+00:00:02 manfred Exp $
 */



/** Build a fuzzy model, based on learning and validation data.
 * 
 * The output file names are created using the input file name
 * as prefix:
 * Writes estimation results to mod_[filename_extension]c[C]_r[R].out,  
 * best models to mod_[filename_extension]c[C]_r[R].mod,
 * R2 to mod_[filename_extension]c[C].r2,
 * and error to mod_[filename_extension]c[C].err
 * @memo
 */
void fzymodel(char* learnfilename, char* validationfilename) throw (Error);


#endif /// #ifndef FZYMODEL_HH
