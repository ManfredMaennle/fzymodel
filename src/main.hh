#ifndef MAIN_HH
#define MAIN_HH

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
 * $Id: main.hh,v 2.9 2020-08-29 10:47:39+00:00:02 manfred Exp $
 */


#ifndef WIN2017
#include <fstream.h>
#else
#include <fstream>
#endif


/** @name main
 */

//@{
/** The main program. 
 *
 * Parses command line, checks command line arguments and then
 * calls the appropriate module (e.g., fzymodel, fzyestim, etc.).
 *
 * @memo
 *
 * @param argc  The number of commandline parameters.
 * @param argv  The commandline parameters.
 *
 * @return The return status of the modeling/simulation (0 = ok, else error).
 *
 * @author  Manfred M\"annle
 * @version $Revision: 2.9 $ $Date: 2020-08-29 10:47:39+00:00:02 $ 
 */
int main(int argc, char* argv[], char* env[]);


/** Print the command line arguments to a stream.
 *
 * @memo
 *
 * @param strm  output file stream
 * @param argc  no. of command line arguments
 * @param argv  command line argument vector
 * @param env   environment vector
 */
void print_options(ofstream& strm, int argc, char* argv[], char* env[]);
//@}

#endif /* #ifndef MAIN_HH */
