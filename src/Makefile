## MIT License
##
## Copyright (c) 1999, 2020 Manfred Maennle
## 
## Permission is hereby granted, free of charge, to any person obtaining a copy
## of this software and associated documentation files (the "Software"), to deal
## in the Software without restriction, including without limitation the rights
## to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
## copies of the Software, and to permit persons to whom the Software is
## furnished to do so, subject to the following conditions:
## 
## The above copyright notice and this permission notice shall be included in all
## copies or substantial portions of the Software.
## 
## THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
## IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
## FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
## AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
## LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
## OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
## SOFTWARE.
##
## $Id: Makefile,v 2.9 2020/08/29 15:18:26 manfred Exp $
##

#FSETS	= trapez4
FSETS	= sigmoid

############################## settings

### compilers and tools
CCPP	= g++
CC	= gcc
YACC	= bison -b y
LEX	= flex
STRIP	= strip
INSTALL	= cp
DOCIFY	= docify
DOCPP	= docxx
LATEX	= pdflatex
RM	= rm -f
LN	= ln -s
CD	= cd


### binary flags 32 / 64 bit
BITS	= 32
#BITS	= 64

ifeq ($(OS),Windows_NT)
  BINFLAGS = -m$(BITS) -DWIN2017
else ### for unix/linux environment
  BINFLAGS = -m$(BITS)
  DOCPP	= docxx
endif


### machine type
#MACH	= "sun4"
#MACH	= "i586"
MACH	= unknown
ifeq ($(OS),Windows_NT)
  MACH = win_x86_$(BITS)bit
else ### for unix/linux environment
  MACH = $(HOSTTYPE)
endif


### RCS release note for ci   <<<<<<<<<<============ change here for new release
#RCSRELEASENOTE = -u2.9 -m"Put under MIT license."
RCSRELEASENOTE = 

### RCS treats suffixes differently in Windows and Unix environments
ifeq ($(OS),Windows_NT)
  RCSSUFF = 
  RCSXOPT = -x,v -zUTC
  RCSPATH = 
else ### for unix/linux environment
  RCSSUFF = ,v
  RCSXOPT = -zUTC
  RCSPATH = RCS/
endif


### compiler (debug/optimization) options
COPT	= -Wall -pedantic -O3 -DTNT_NO_BOUNDS_CHECK -DNDEBUG
#COPT	= -Wall -pedantic -O2 -DTNT_NO_BOUNDS_CHECK -DNDEBUG
#COPT	= -Wall -pedantic -O -DTNT_NO_BOUNDS_CHECK -DNDEBUG
#COPT	= -Wall -pedantic -DTNT_NO_BOUNDS_CHECK -DNDEBUG
#COPT	= -Wall -pedantic
#COPT	= -Wall -pedantic -g -DYYDEBUG

### conditional compilation flags
### $(MAKE_CFLAGS) is taken from the shell environment 
### (it is to be set by the calling script before recompilation)
#CFLAGS	= $(MAKE_CFLAGS) -DLONGDOUBLE -DEXACT_SIGMOID $(BINFLAGS) 
#CFLAGS	= $(MAKE_CFLAGS) -DDOUBLE $(BINFLAGS) 
CFLAGS	= $(MAKE_CFLAGS) -DFLOAT $(BINFLAGS) 

ifeq ($(FSETS),trapez4)
  CCFLAGS = $(CFLAGS) -DTRAPEZOIDAL_FSETS
else
  CCFLAGS = $(CFLAGS)
endif

### linker flags
LDFLAGS	= -Wall $(BINFLAGS)

### flags for lex and yacc
LXFLAGS	= -t
YFLAGS	= -d -v

### additional include paths
INCPATH	=
ifeq ($(MACH),sun4)
INCPATH	= -I../include/TNT -I../include/flex
endif
ifeq ($(OS),Windows_NT)
INCPATH = -I../include/TNT_126 -I/usr/local/include -I../../../../programs-usb/MinGW/msys/1.0/include -I../../../../programs-usb/MinGW/lib/gcc/mingw32/4.6.2/include/c++/parallel
#INCPATH = -I../include/TNT_126 -I/usr/local/include -I../../../../programs-usb/MinGW/msys/1.0/include -I./parallel
#INCPATH = -I../include/TNT_126 -I/usr/local/include -I../../../../programs-usb/MinGW/msys/1.0/include
endif



### additional library paths
LIBPATH	= 

### libraries
#LIBS	= -lstdc++ -lglib -liostream -lfl -lm
LIBS	= -lm


### suffix for binary executable
SUFFIX = 
ifeq ($(OS),Windows_NT)
  SUFFIX = .exe
else ### Unix
  SUFFIX = 
endif

### destination paths
DESTDIR	= ..
DESTBIN	= $(DESTDIR)/bin/$(MACH)
DESTDOC = $(DESTDIR)/doc


###########################################################################
############### you should not need to change anything below ##############
###########################################################################


### executables
PROGS	= fzymodel$(SUFFIX)

### common objects
OBJS	= main.o fzymodel.o fzyestim.o fzy2sets.o fzymkdat.o fzynorml.o \
	  fmodel.o svd.o minimize.o param.o fzy_prs.o fzy_lex.o \
	  data.o data_lex.o global.o #page_hinkley.o


############################## to do

#all: bin
all: install

install: cleanall bin

compile: fzymodel$(SUFFIX)
#	$(RM) fzyestim$(SUFFIX) fzysimul$(SUFFIX) fzy2sets$(SUFFIX) fzymkdat$(SUFFIX) fzynorml$(SUFFIX)
#	$(LN) fzymodel$(SUFFIX) fzynorml$(SUFFIX)
#	$(LN) fzymodel$(SUFFIX) fzymkdat$(SUFFIX)
#	$(LN) fzymodel$(SUFFIX) fzy2sets$(SUFFIX)
#	$(LN) fzymodel$(SUFFIX) fzyestim$(SUFFIX)
#	$(LN) fzymodel$(SUFFIX) fzysimul$(SUFFIX)

bin: compile remove
	strip $(PROGS)
	$(INSTALL) fzymodel$(SUFFIX) $(DESTBIN)/fzymodel_$(FSETS)$(SUFFIX)
#	$(LN) $(DESTBIN)/fzymodel_$(FSETS)$(SUFFIX) $(DESTBIN)/fzynorml_$(FSETS)$(SUFFIX)
#	$(LN) $(DESTBIN)/fzymodel_$(FSETS)$(SUFFIX) $(DESTBIN)/fzymkdat_$(FSETS)$(SUFFIX)
#	$(LN) $(DESTBIN)/fzymodel_$(FSETS)$(SUFFIX) $(DESTBIN)/fzy2sets_$(FSETS)$(SUFFIX)
#	$(LN) $(DESTBIN)/fzymodel_$(FSETS)$(SUFFIX) $(DESTBIN)/fzyestim_$(FSETS)$(SUFFIX)
#	$(LN) $(DESTBIN)/fzymodel_$(FSETS)$(SUFFIX) $(DESTBIN)/fzysimul_$(FSETS)$(SUFFIX)
	$(INSTALL) fzymodel$(SUFFIX) $(DESTBIN)/fzynorml_$(FSETS)$(SUFFIX)
	$(INSTALL) fzymodel$(SUFFIX) $(DESTBIN)/fzymkdat_$(FSETS)$(SUFFIX)
	$(INSTALL) fzymodel$(SUFFIX) $(DESTBIN)/fzy2sets_$(FSETS)$(SUFFIX)
	$(INSTALL) fzymodel$(SUFFIX) $(DESTBIN)/fzyestim_$(FSETS)$(SUFFIX)
	$(INSTALL) fzymodel$(SUFFIX) $(DESTBIN)/fzysimul_$(FSETS)$(SUFFIX)

remove: 
	$(RM) $(DESTBIN)/fzymodel_$(FSETS)$(SUFFIX)
	$(RM) $(DESTBIN)/fzyestim_$(FSETS)$(SUFFIX)
	$(RM) $(DESTBIN)/fzysimul_$(FSETS)$(SUFFIX)
	$(RM) $(DESTBIN)/fzy2sets_$(FSETS)$(SUFFIX)
	$(RM) $(DESTBIN)/fzymkdat_$(FSETS)$(SUFFIX)
	$(RM) $(DESTBIN)/fzynorml_$(FSETS)$(SUFFIX)

uninstall: remove

doc:
#	$(DOCPP) -p -H -G -d $(DESTDOC)/html -B $(DESTDOC)/doc/footline.html $(DESTDOC)/doc/doc.dxx
	$(DOCPP) -p -t -o $(DESTDOC)/fzymodeldoc.tex $(DESTDOC)/doc.dxx
	$(CD) $(DESTDOC) && $(LATEX) fzymodeldoc.tex
	$(CD) $(DESTDOC) && $(LATEX) fzymodeldoc.tex

clean:
	$(RM) *~ *.o *_lex.cc *_lex.c *.output *_prs.cc *_prs.h *.log core $(DESTDOC)/*.log $(DESTDOC)/*.aux $(DESTDOC)/*.tex

cleanall: clean 
	$(RM) $(PROGS)
	$(RM) fzyestim$(SUFFIX) fzysimul$(SUFFIX) fzy2sets$(SUFFIX) fzymkdat$(SUFFIX) fzynorml$(SUFFIX)
	$(RM) $(DESTBIN)/fzy*

ci: cleanall
	ci $(RCSXOPT) $(RCSRELEASENOTE) data.cc
	ci $(RCSXOPT) $(RCSRELEASENOTE) data.hh
	ci $(RCSXOPT) $(RCSRELEASENOTE) data_lex.h
	ci $(RCSXOPT) $(RCSRELEASENOTE) data_lex.l
	ci $(RCSXOPT) $(RCSRELEASENOTE) fmodel.cc
	ci $(RCSXOPT) $(RCSRELEASENOTE) fmodel.hh
	ci $(RCSXOPT) $(RCSRELEASENOTE) funct.cc
	ci $(RCSXOPT) $(RCSRELEASENOTE) funct.hh
	ci $(RCSXOPT) $(RCSRELEASENOTE) fzy2sets.cc
	ci $(RCSXOPT) $(RCSRELEASENOTE) fzy2sets.hh
	ci $(RCSXOPT) $(RCSRELEASENOTE) fzy_lex.h
	ci $(RCSXOPT) $(RCSRELEASENOTE) fzy_lex.l
	ci $(RCSXOPT) $(RCSRELEASENOTE) fzy_prs.y
	ci $(RCSXOPT) $(RCSRELEASENOTE) fzyestim.cc
	ci $(RCSXOPT) $(RCSRELEASENOTE) fzyestim.hh
	ci $(RCSXOPT) $(RCSRELEASENOTE) fzymkdat.cc
	ci $(RCSXOPT) $(RCSRELEASENOTE) fzymkdat.hh
	ci $(RCSXOPT) $(RCSRELEASENOTE) fzymodel.cc
	ci $(RCSXOPT) $(RCSRELEASENOTE) fzymodel.hh
	ci $(RCSXOPT) $(RCSRELEASENOTE) fzynorml.cc
	ci $(RCSXOPT) $(RCSRELEASENOTE) fzynorml.hh
	ci $(RCSXOPT) $(RCSRELEASENOTE) global.cc
	ci $(RCSXOPT) $(RCSRELEASENOTE) global.hh
	ci $(RCSXOPT) $(RCSRELEASENOTE) main.cc
	ci $(RCSXOPT) $(RCSRELEASENOTE) main.hh
	ci $(RCSXOPT) $(RCSRELEASENOTE) minimize.cc
	ci $(RCSXOPT) $(RCSRELEASENOTE) minimize.hh
	ci $(RCSXOPT) $(RCSRELEASENOTE) page_hinkley.cc
	ci $(RCSXOPT) $(RCSRELEASENOTE) page_hinkley.hh
	ci $(RCSXOPT) $(RCSRELEASENOTE) param.cc
	ci $(RCSXOPT) $(RCSRELEASENOTE) param.hh
	ci $(RCSXOPT) $(RCSRELEASENOTE) svd.cc
	ci $(RCSXOPT) $(RCSRELEASENOTE) svd.hh


co:
	co $(RCSXOPT) $(RCSPATH)data.cc$(RCSSUFF)
	co $(RCSXOPT) $(RCSPATH)data.hh$(RCSSUFF)
	co $(RCSXOPT) $(RCSPATH)data_lex.h$(RCSSUFF)
	co $(RCSXOPT) $(RCSPATH)data_lex.l$(RCSSUFF)
	co $(RCSXOPT) $(RCSPATH)fmodel.cc$(RCSSUFF)
	co $(RCSXOPT) $(RCSPATH)fmodel.hh$(RCSSUFF)
	co $(RCSXOPT) $(RCSPATH)funct.cc$(RCSSUFF)
	co $(RCSXOPT) $(RCSPATH)funct.hh$(RCSSUFF)
	co $(RCSXOPT) $(RCSPATH)fzy2sets.cc$(RCSSUFF)
	co $(RCSXOPT) $(RCSPATH)fzy2sets.hh$(RCSSUFF)
	co $(RCSXOPT) $(RCSPATH)fzy_lex.h$(RCSSUFF)
	co $(RCSXOPT) $(RCSPATH)fzy_lex.l$(RCSSUFF)
	co $(RCSXOPT) $(RCSPATH)fzy_prs.y$(RCSSUFF)
	co $(RCSXOPT) $(RCSPATH)fzyestim.cc$(RCSSUFF)
	co $(RCSXOPT) $(RCSPATH)fzyestim.hh$(RCSSUFF)
	co $(RCSXOPT) $(RCSPATH)fzymkdat.cc$(RCSSUFF)
	co $(RCSXOPT) $(RCSPATH)fzymkdat.hh$(RCSSUFF)
	co $(RCSXOPT) $(RCSPATH)fzymodel.cc$(RCSSUFF)
	co $(RCSXOPT) $(RCSPATH)fzymodel.hh$(RCSSUFF)
	co $(RCSXOPT) $(RCSPATH)fzynorml.cc$(RCSSUFF)
	co $(RCSXOPT) $(RCSPATH)fzynorml.hh$(RCSSUFF)
	co $(RCSXOPT) $(RCSPATH)global.cc$(RCSSUFF)
	co $(RCSXOPT) $(RCSPATH)global.hh$(RCSSUFF)
	co $(RCSXOPT) $(RCSPATH)main.cc$(RCSSUFF)
	co $(RCSXOPT) $(RCSPATH)main.hh$(RCSSUFF)
	co $(RCSXOPT) $(RCSPATH)minimize.cc$(RCSSUFF)
	co $(RCSXOPT) $(RCSPATH)minimize.hh$(RCSSUFF)
	co $(RCSXOPT) $(RCSPATH)page_hinkley.cc$(RCSSUFF)
	co $(RCSXOPT) $(RCSPATH)page_hinkley.hh$(RCSSUFF)
	co $(RCSXOPT) $(RCSPATH)param.cc$(RCSSUFF)
	co $(RCSXOPT) $(RCSPATH)param.hh$(RCSSUFF)
	co $(RCSXOPT) $(RCSPATH)svd.cc$(RCSSUFF)
	co $(RCSXOPT) $(RCSPATH)svd.hh$(RCSSUFF)


co-l: cleanall
	co -l $(RCSXOPT) $(RCSPATH)data.cc$(RCSSUFF)
	co -l $(RCSXOPT) $(RCSPATH)data.hh$(RCSSUFF)
	co -l $(RCSXOPT) $(RCSPATH)data_lex.h$(RCSSUFF)
	co -l $(RCSXOPT) $(RCSPATH)data_lex.l$(RCSSUFF)
	co -l $(RCSXOPT) $(RCSPATH)fmodel.cc$(RCSSUFF)
	co -l $(RCSXOPT) $(RCSPATH)fmodel.hh$(RCSSUFF)
	co -l $(RCSXOPT) $(RCSPATH)funct.cc$(RCSSUFF)
	co -l $(RCSXOPT) $(RCSPATH)funct.hh$(RCSSUFF)
	co -l $(RCSXOPT) $(RCSPATH)fzy2sets.cc$(RCSSUFF)
	co -l $(RCSXOPT) $(RCSPATH)fzy2sets.hh$(RCSSUFF)
	co -l $(RCSXOPT) $(RCSPATH)fzy_lex.h$(RCSSUFF)
	co -l $(RCSXOPT) $(RCSPATH)fzy_lex.l$(RCSSUFF)
	co -l $(RCSXOPT) $(RCSPATH)fzy_prs.y$(RCSSUFF)
	co -l $(RCSXOPT) $(RCSPATH)fzyestim.cc$(RCSSUFF)
	co -l $(RCSXOPT) $(RCSPATH)fzyestim.hh$(RCSSUFF)
	co -l $(RCSXOPT) $(RCSPATH)fzymkdat.cc$(RCSSUFF)
	co -l $(RCSXOPT) $(RCSPATH)fzymkdat.hh$(RCSSUFF)
	co -l $(RCSXOPT) $(RCSPATH)fzymodel.cc$(RCSSUFF)
	co -l $(RCSXOPT) $(RCSPATH)fzymodel.hh$(RCSSUFF)
	co -l $(RCSXOPT) $(RCSPATH)fzynorml.cc$(RCSSUFF)
	co -l $(RCSXOPT) $(RCSPATH)fzynorml.hh$(RCSSUFF)
	co -l $(RCSXOPT) $(RCSPATH)global.cc$(RCSSUFF)
	co -l $(RCSXOPT) $(RCSPATH)global.hh$(RCSSUFF)
	co -l $(RCSXOPT) $(RCSPATH)main.cc$(RCSSUFF)
	co -l $(RCSXOPT) $(RCSPATH)main.hh$(RCSSUFF)
	co -l $(RCSXOPT) $(RCSPATH)minimize.cc$(RCSSUFF)
	co -l $(RCSXOPT) $(RCSPATH)minimize.hh$(RCSSUFF)
	co -l $(RCSXOPT) $(RCSPATH)page_hinkley.cc$(RCSSUFF)
	co -l $(RCSXOPT) $(RCSPATH)page_hinkley.hh$(RCSSUFF)
	co -l $(RCSXOPT) $(RCSPATH)param.cc$(RCSSUFF)
	co -l $(RCSXOPT) $(RCSPATH)param.hh$(RCSSUFF)
	co -l $(RCSXOPT) $(RCSPATH)svd.cc$(RCSSUFF)
	co -l $(RCSXOPT) $(RCSPATH)svd.hh$(RCSSUFF)

rcs-u: cleanall
	rcs -u  $(RCSXOPT) $(RCSPATH)data.cc$(RCSSUFF)
	rcs -u  $(RCSXOPT) $(RCSPATH)data.hh$(RCSSUFF)
	rcs -u  $(RCSXOPT) $(RCSPATH)data_lex.h$(RCSSUFF)
	rcs -u  $(RCSXOPT) $(RCSPATH)data_lex.l$(RCSSUFF)
	rcs -u  $(RCSXOPT) $(RCSPATH)fmodel.cc$(RCSSUFF)
	rcs -u  $(RCSXOPT) $(RCSPATH)fmodel.hh$(RCSSUFF)
	rcs -u  $(RCSXOPT) $(RCSPATH)funct.cc$(RCSSUFF)
	rcs -u  $(RCSXOPT) $(RCSPATH)funct.hh$(RCSSUFF)
	rcs -u  $(RCSXOPT) $(RCSPATH)fzy2sets.cc$(RCSSUFF)
	rcs -u  $(RCSXOPT) $(RCSPATH)fzy2sets.hh$(RCSSUFF)
	rcs -u  $(RCSXOPT) $(RCSPATH)fzy_lex.h$(RCSSUFF)
	rcs -u  $(RCSXOPT) $(RCSPATH)fzy_lex.l$(RCSSUFF)
	rcs -u  $(RCSXOPT) $(RCSPATH)fzy_prs.y$(RCSSUFF)
	rcs -u  $(RCSXOPT) $(RCSPATH)fzyestim.cc$(RCSSUFF)
	rcs -u  $(RCSXOPT) $(RCSPATH)fzyestim.hh$(RCSSUFF)
	rcs -u  $(RCSXOPT) $(RCSPATH)fzymkdat.cc$(RCSSUFF)
	rcs -u  $(RCSXOPT) $(RCSPATH)fzymkdat.hh$(RCSSUFF)
	rcs -u  $(RCSXOPT) $(RCSPATH)fzymodel.cc$(RCSSUFF)
	rcs -u  $(RCSXOPT) $(RCSPATH)fzymodel.hh$(RCSSUFF)
	rcs -u  $(RCSXOPT) $(RCSPATH)fzynorml.cc$(RCSSUFF)
	rcs -u  $(RCSXOPT) $(RCSPATH)fzynorml.hh$(RCSSUFF)
	rcs -u  $(RCSXOPT) $(RCSPATH)global.cc$(RCSSUFF)
	rcs -u  $(RCSXOPT) $(RCSPATH)global.hh$(RCSSUFF)
	rcs -u  $(RCSXOPT) $(RCSPATH)main.cc$(RCSSUFF)
	rcs -u  $(RCSXOPT) $(RCSPATH)main.hh$(RCSSUFF)
	rcs -u  $(RCSXOPT) $(RCSPATH)minimize.cc$(RCSSUFF)
	rcs -u  $(RCSXOPT) $(RCSPATH)minimize.hh$(RCSSUFF)
	rcs -u  $(RCSXOPT) $(RCSPATH)page_hinkley.cc$(RCSSUFF)
	rcs -u  $(RCSXOPT) $(RCSPATH)page_hinkley.hh$(RCSSUFF)
	rcs -u  $(RCSXOPT) $(RCSPATH)param.cc$(RCSSUFF)
	rcs -u  $(RCSXOPT) $(RCSPATH)param.hh$(RCSSUFF)
	rcs -u  $(RCSXOPT) $(RCSPATH)svd.cc$(RCSSUFF)
	rcs -u  $(RCSXOPT) $(RCSPATH)svd.hh$(RCSSUFF)



############################## link

fzymodel$(SUFFIX): $(OBJS)
	$(CCPP) -o $@ $(COPT) $(LDFLAGS) $(LIBPATH) $(OBJS) $(LIBS)


############################## compilation rules

%.o: %.cc
	$(CCPP) $(CCFLAGS) $(COPT) $(INCPATH) -c $<

%.o: %.c
	$(CC) $(CCFLAGS) $(COPT) $(INCPATH) -c $<

%.cc: %.l
	@$(RM) -f $@
	$(LEX) $(LXFLAGS) $< > $@

fzy_prs.cc: fzy_prs.y
	$(YACC) $(YFLAGS) -p fzy_ $< -o fzy_prs.c
	mv fzy_prs.c fzy_prs.cc

#wb_parse.c: wb_parse.y
#	$(YACC) $(YFLAGS) -p wb_ $< -o $@
#%prs.cc: %prs.y
#	$(YACC) $(YFLAGS) -p `basename $@ prs.cc` $< -o $@


############################## dependencies

global.o:	Makefile global.hh global.cc
param.o:	Makefile global.hh param.hh param.cc
data_lex.o:	Makefile data_lex.h data_lex.l
data.o: 	Makefile global.hh param.hh data_lex.h data.hh data.cc 
svd.o:		Makefile global.hh param.hh svd.hh svd.cc
page_hinkley.o:	Makefile global.hh page_hinkley.hh page_hinkley.cc
minimize.o:	Makefile global.hh param.hh fmodel.hh funct.hh \
		minimize.hh minimize.cc
fmodel.o: 	Makefile global.hh param.hh svd.hh data.hh \
		funct.hh minimize.hh fmodel.hh fmodel.cc
funct.o:	Makefile global.hh param.hh funct.hh funct.cc
fzy_lex.o:	Makefile fzy_prs.y fzy_lex.h fzy_lex.l 
fzy_prs.o:	Makefile global.hh param.hh fmodel.hh fzy_lex.h fzy_prs.y 
main.o: 	Makefile global.hh param.hh data.hh fzymodel.hh fzy2sets.hh \
	     	fzymkdat.hh fzyestim.hh fzynorml.hh main.hh main.cc
fzymodel.o:	Makefile global.hh param.hh data.hh fmodel.hh main.hh \
		funct.hh minimize.hh fzymodel.hh fzymodel.cc
fzyestim.o:	Makefile global.hh param.hh data.hh fmodel.hh main.hh \
		fzy_lex.h fzyestim.hh fzyestim.cc
fzy2sets.o:	Makefile global.hh param.hh fmodel.hh main.hh \
		fzy_lex.h fzy2sets.hh fzy2sets.cc
fzymkdat.o:	Makefile global.hh param.hh main.hh data.hh fmodel.hh \
		fzymkdat.hh fzymkdat.cc
fzynorml.o:	Makefile global.hh param.hh main.hh data.hh fmodel.hh \
		fzynorml.hh fzynorml.cc
