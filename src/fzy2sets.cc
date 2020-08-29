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
 * $Id: fzy2sets.cc,v 2.9 2020-08-29 10:47:36+00:00:02 manfred Exp $
 */


#include "global.hh"
#include "fmodel.hh"

#include "fzy2sets.hh"


const Real xoffset = 1.5 / 0.8 * mu_right; 
const Real intervall = 1.5 / 0.8 * (mu_left - mu_right);


string texify(const string& s) {
  string t = "";
  for (size_t k = 0; k < s.size(); ++k) {
    char c = s[k];
    if (c == '_') {
      t += "\\_";
    }
    else {
      t += c;
    }
  }
  return t;
}


void fzy2sets(char* infilename, char* normfilename) throw (Error) {
	string gnuplotpreface = "set size 0.36, 0.27\n";
	gnuplotpreface += "set xrange [-0.75:0.75]\n";
	gnuplotpreface += "set xtics -0.5, 0.5, 0.5\n";
	gnuplotpreface += "set yrange [0:1]\n";
	gnuplotpreface += "set ytics 0, 0.5, 1\n";
	gnuplotpreface += "set xlabel \"\"\n";
	gnuplotpreface += "set ylabel \"\"\n";
	gnuplotpreface += "set term postscript eps \"Times-Roman\" 20\n";

	string texpreface = "%\n";	
	texpreface += "\\def\\true{\\yes}\n";
	texpreface += "\\def\\false{\\no}\n";
	texpreface += "\\ifx\\wholedocument\\true\n";
	texpreface += "\\typeout{ *** mod-file is input file only: begin ***}\n";
	texpreface += "\\else\n";
	texpreface += "\\input{tex_modheader}\n";
	texpreface += "%\\NeedsTeXFormat{LaTeX2e}\n";
	texpreface += "%\\documentclass[12pt,a4paper]{report}\n";
	texpreface += "%%\\documentclass[12pt,a4paper,twoside,twocolumn]{report}\n";
	texpreface += "%%% page layout\n";
	texpreface += "%%\\pagestyle{empty}\n";
	texpreface += "%\\pagestyle{plain}\n";
	texpreface += "%%% paragraph layout\n";
	texpreface += "%\\setlength{\\parindent}{0pt}\n";
	texpreface += "%\\setlength{\\parskip}{5pt plus 2pt minus 1pt}\n";
	texpreface += "%\\frenchspacing\n";
	texpreface += "%\\sloppy\n";
	texpreface += "%\n%\n";
	texpreface += "\\input{tex_common}\n";
	texpreface += "%\\def\\FZYpath{./}\n";
	texpreface += "%\\setlength{\\unitlength}{1mm}\n";
	texpreface += "%\\def\\FZYtopvspace{-4ex}\n";
	texpreface += "%\\def\\FZYbottomvspace{4ex}\n";
	texpreface += "%\\def\\FZYpicturesizex{32}\n";
	texpreface += "%\\def\\FZYpicturesizey{24}\n";
	texpreface += "%\\def\\FZYpictureoriginx{2}\n";  // shift left by 2 mm
	texpreface += "%\\def\\FZYpictureoriginy{12}\n"; // shift down by 12 mm
	texpreface += "%\\def\\FZYepsoriginx{0}\n";
	texpreface += "%\\def\\FZYepsoriginy{0}\n";
	texpreface += "%\\def\\FZYwidth{36mm}\n";
	texpreface += "%\\def\\FZYheight{24mm}\n";
	texpreface += "%\\def\\FZYpicturehspaceleft{-2mm}\n";
	texpreface += "%\\def\\FZYpicturehspaceright{-2mm}\n";
	texpreface += "%\n%\n";
	texpreface += "\\begin{document}\n";
	texpreface += "\\fi\n";
	texpreface += "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n";
	texpreface += "%\n";
	texpreface += "%\\begin{figure}[!ht]\n";

	string texappendix = "%\n%\n";
	texappendix += "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n";
	texappendix += "\\ifx\\wholedocument\\true\n";
	texappendix += "\\typeout{ *** mod-file is input file only: end ***}\n";
	texappendix += "\\else\n";
	texappendix += "\\input{tex_close}\n";
	texappendix += "\\end{document}\n";
	texappendix += "\\fi\n";
	texappendix += "%%% EOF\n";

	FModel fmodel;
	fmodel.load(infilename);
	if (! GLOBAL::quiet) {
		cout << "loaded model:\n\n" << fmodel << endl;
	}  
	Data normalization;
	if (GLOBAL::denormalize) {
		normalization.load(normfilename);
	}

	int length = strlen(infilename) - 4;
	string basefilename;
	if (length < 1) {
		basefilename = (string)infilename;
	} 
	else {
		basefilename = ((string)infilename).substr(0, length);
	}
	// basefilename += GLOBAL::filename_extension;
	string texfilename = basefilename + tex_suffix;
	ofstream texfile(texfilename.c_str());
	if (! texfile) {
		throw FileOpenError(texfilename);
	}
	string gnuplotfilename = basefilename + GLOBAL::gnuplot_file_extension;
	ofstream gnuplotfile(gnuplotfilename.c_str());
	if (! gnuplotfile) {
		throw FileOpenError(gnuplotfilename);
	}
	texfile.precision(GLOBAL::tex_precision);
	texfile << "%%% " << texfilename << "\n";
	texfile << texpreface;
	texfile << "\\vspace{\\FZYtopvspace}\n%\n";
	for (size_t r = 0; r < fmodel.rdim(); ++r) {
		texfile << "$\\mathtt{R_" << itos(r+1) << "}$:\n"; 
		// /// *** print premise
		size_t u = 0;
		int n_printed_fsets = 0;
		const Premise premise = fmodel.frules(r).prem();
		Premise::const_iterator pprem = premise.begin();
		while (pprem != premise.end()) {
			FSet* left = *(pprem++);
			FSet* right = *(pprem++);
			if ( (left != NULL) || (right != NULL) ) {
				string plotname = basefilename + "_r" + itos(r+1) + "_u" + itos(u+1);
				string pixlfilename = plotname + pixl_suffix;
				ofstream pixlfile(pixlfilename.c_str());
				if (! pixlfile) {
					throw FileOpenError(pixlfilename);
				}
				pixlfile << "### " << pixlfilename << endl;
				if (n_printed_fsets == 0) {
					texfile << "\\verb|if| ";
				}
				else {
					texfile << "\\verb|and| ";
				}
				++n_printed_fsets;
				texfile << "$u_{" << itos(u+1) << "}$ \\verb|is|\n";
				texfile << "  \\hspace{\\FZYpicturehspaceleft}\n";
				texfile << "  \\begin{picture}(\\FZYpicturesizex,\\FZYpicturesizey)";
				texfile << "(\\FZYpictureoriginx,\\FZYpictureoriginy)\n";
				texfile << "  \\put(\\FZYepsoriginx,\\FZYepsoriginy)";
				texfile << "{\\includegraphics[width=\\FZYwidth,height=\\FZYheight]";
				texfile << "{\\FZYpath/" << plotname << "}}\n";
				texfile << "  \\end{picture}\n";
				texfile << "  \\hspace{\\FZYpicturehspaceleft}\n";
				texfile << flush;
				Real xm = 0.0;
				Real xs = 1.0;
				Real ym = 0.0;
				Real ys = 1.0;
// 				if (GLOBAL::denormalize) {
// 	  	  			xm = normalization.scale_shift()[u];
// 	  	  			xs = normalization.scale_factor()[u];
// 	  	  			ym = *(normalization.scale_shift().end() - 1);
// 	  	  			ys = *(normalization.scale_factor().end() - 1);
// 	  	  			if (xs == 0.0) xs = 1.0;
// 	  	  			if (ys == 0.0) ys = 1.0;
// 					}
				// *** write the pixels
				tracemsg(100, "in fzy2sets(): start writing n pixels: ", GLOBAL::n_pixels);
				for (size_t k = 0; k <= GLOBAL::n_pixels; ++k) {
					Real x = xoffset + intervall * (Real)k / (Real)GLOBAL::n_pixels;
					Real sum_F = 0.0;
					if (GLOBAL::global_fset_value) {
						Premise::const_iterator p = premise.begin();
						while (p != premise.end()) {
							FSet* fl = *(p++);
							FSet* fr = *(p++);
							if ((fr != NULL) || (fl != NULL)) {
								Real sum_Fr = 0.0;
								if (fr == NULL) {
									sum_Fr = (fl->F(x) + ym)/ys;
								}
								else if (fl == NULL) {
									sum_Fr = (fr->F(x) + ym)/ys;
								}
								else {
									sum_Fr = (fl->F(x) + ym)/ys * (fr->F(x) + ym)/ys;
								}
								sum_F += sum_Fr;
							}
						} // / end while (p != premise.end()) 
					}
					else { // if (not GLOBAL::global_fset_value) 
						sum_F = 1.0;
					}
					Real Fi = 0.0;
					if (sum_F == 0.0) {
						Fi = 0.0;
					}
					else {
						if (right == NULL) {
							Fi = ((left->F(x))/sum_F+ym)/ys;
						}
						else if (left == NULL) {
							Fi = ((right->F(x))/sum_F+ym)/ys;
						}
						else {
							Fi = ((left->F(x))/sum_F+ym)/ys * ((right->F(x))/sum_F+ym)/ys;
						}
					}
					pixlfile << (x+xm)/xs << " " << Fi << endl;
				} // end for (size_t k = 0; k <= GLOBAL::n_pixels; ++k) 
				pixlfile.close();
				tracemsg(100, "in fzy2sets(): writing pixelfile", "OK");	
   
				// /// print gnuplot
				string epsfilename = plotname + eps_suffix;
				/*
				ofstream epsfile(epsfilename.c_str());
				if (! epsfile) {
					throw FileOpenError(epsfilename);
				}
				*/
				string gnuplotcall = gnuplotpreface;
				if (GLOBAL::denormalize) {
					Real xm = normalization.scale_shift()[u];
					Real xs = normalization.scale_factor()[u];
					if (xs == 0.0) xs = 1.0;	  
					gnuplotcall += "set xtics (\"";
					gnuplotcall += dtos2(-0.5/xs + xm);
					gnuplotcall += "\" -0.5, \"";
					gnuplotcall += dtos2(0.5/xs + xm);
					gnuplotcall += "\" 0.5)\n";
				}
				gnuplotcall += "set output \"" + epsfilename + "\"\n";
				gnuplotcall += "plot \"" + pixlfilename + "\" title \"\" w l\n";
				gnuplotfile << "##### new eps\n\n" << gnuplotcall << "\n\n\n" << flush;
			} // end if ( (left != NULL) || (right != NULL) ) 
			else {
				// /// nothing to do 
				tracemsg(200, "in fzy2sets(): print premises", "left and right == NULL");
			}
			++u;
		} // end while (pprem != premise.end())
		tracemsg(100, "in fzy2sets(): end printing premises", "OK");	
		if (u == 0) {
			texfile << "TRUE\n";
		}
		
		// /// *** print consequence
		tracemsg(100, "in fzy2sets(): start printing consequence", "OK");	
		Real ym = 0.0;
		Real ys = 1.0;
		tracemsg(200, "in fzy2sets(): GLOBAL::denormalize setting", GLOBAL::denormalize);	
		if (GLOBAL::denormalize) {
			ym = *(normalization.scale_factor().end() - 1);
			ys = *(normalization.scale_shift().end() - 1);
		}
		tracemsg(200, "in fzy2sets(): denormalize step", "OK");	
		if (ys == 0.0) ys = 1.0;
		Real sum = *(fmodel.frules(r).cons().begin());
		Consequence::const_iterator pcons = fmodel.frules(r).cons().begin() + 1;
		size_t c = 0;    
		if (GLOBAL::denormalize) {
			while (pcons != fmodel.frules(r).cons().end()) {
				tracemsg(300, "in fzy2sets(): consequence first loop, c", c);
				sum -= (*pcons) * (normalization.scale_shift()[c]) * (normalization.scale_factor()[c]);
				++pcons;
				++c;
			}
		}
		tracemsg(200, "in fzy2sets(): consequence first loop", "OK");
		texfile << "\\verb|then| $f_" << itos(r+1) << " = " << sum/ys+ym;
		pcons = fmodel.frules(r).cons().begin() + 1;
		c = 0;
		while (pcons != fmodel.frules(r).cons().end()) {
			Real scale_value = 1.0;
			if (GLOBAL::denormalize) {
				scale_value = normalization.scale_factor()[c];
			}
			texfile << "\n  + " 
				<< (*(pcons) * scale_value / ys)
				<< " u_{" << itos(c+1) << "}";
			++pcons;
			++c;
		}
		texfile << "$ \\par\n%\n";
		tracemsg(100, "in fzy2sets(): end printing consequence", "OK");
	} // end for (size_t r = 0; r < fmodel.rdim(); ++r) 
		
	texfile << "\\vspace{\\FZYbottomvspace}\n";
	texfile << "%\\caption{\\label{fig:" << basefilename << "}";
	texfile << texify(basefilename) << "}\n";
	texfile << "%\\end{figure}\n%\n";
	texfile << texappendix;
	texfile.close();
	gnuplotfile.close();
	if (GLOBAL::execute_gnuplot_system_call) {
		string syscall = "gnuplot " + gnuplotfilename;
		tracemsg(100, "in fzy2sets(): starting gnuplot system call: ", syscall.c_str());	
		int syscallresult = system(syscall.c_str());
		tracemsg(100, "in fzy2sets(): result code of gnuplot system call: ", syscallresult);	
		if (syscallresult) {
			string msg = "warning: error in gnuplot call for creation of eps from file " + gnuplotfilename 
			  + ". If eps files are needed for FSet visualization, please install gnuplot.";
			throw SystemCallError(msg);
		}
	}
	return;
}
