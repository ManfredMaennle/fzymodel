
/*
 * fzy_prs.y -- a parser for fuzzy models
 *
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
 *
 * $Id: fzy_prs.y,v 2.9 2020-08-29 10:47:37+00:00:02 manfred Exp $
 *
 */


%{ /**************** C declarations ***************************************/

//#WIN2017 #include <fstream.h>

#include "fmodel.hh"
#include "fzy_lex.h"


/*** modify parser interface ***/

#define YYPARSE_PARAM fmodel


/*** local declarations for this file ***/

/* called by fzy_parse() on error */
//#WIN2017 int  fzy_error(char* error_type);
int  fzy_error(const string& error_type);


/*** local variable definitions ***/

vector<size_t> history;
vector<Real> fsetvect;
vector<Real> consvect;
size_t rdim = 0;
size_t udim = 0;
size_t cdim = 0;
size_t sdim = 0;
size_t worst_rule = 0; 

size_t fsetindex = 0;
size_t consindex = 0;
size_t ruleindex = 0;

int stringpos;


%} /**************** parser declarations **********************************/

%token T_FZY_INT
%token T_FZY_NUMBER
%token T_FZY_VECTOR
%token T_FZY_STRING
%token T_FZY_ASSIGN
%token T_FZY_RDIM
%token T_FZY_UDIM
%token T_FZY_CDIM
%token T_FZY_SDIM
%token T_FZY_WORST_RULE
%token T_FZY_LEARNFILENAME
%token T_FZY_VALIDATIONFILENAME
%token T_FZY_HISTORY
%token T_FZY_SETS
%token T_FZY_NULL
%token T_FZY_CONS
%token T_FZY_RULES
%token T_FZY_SET_BEGIN
%token T_FZY_CONS_BEGIN
%token T_FZY_RULE_BEGIN
%token T_FZY_NEWLINE
%token T_FZY_EOF
%token T_FZY_ERROR


%% /***************** grammer rules and actions ************************/

input: empty {
  // cerr << "warning: fzy file empty\n";
  YYACCEPT;
} | init history learnfn validfn fsets fcons frules T_FZY_EOF { 
  YYACCEPT; 
};
 
init: rdim udim cdim sdim worst_rule {
  if (rdim < 1)
    return fzy_error("rdim too small");
  if (udim < 1)
    return fzy_error("udim too small");
  if (cdim < 1)
    return fzy_error("cdim too small");
  if (cdim > udim+1)
    return fzy_error("cdim > udim+1");
  if (sdim != 2*(rdim-1))
    return fzy_error("sdim != 2*(rdim-1)");
  if (worst_rule < 0)
    return fzy_error("worst_rule too small");
  if (worst_rule >= rdim)
    return fzy_error("worst_rule >= rdim");
  history.resize(sdim);
  fsetvect.resize(8);
  consvect.resize(4 * cdim);
  ((FModel*)fmodel)->worst_rule() = worst_rule; 
};

rdim: T_FZY_RDIM T_FZY_ASSIGN T_FZY_INT T_FZY_NEWLINE {
  rdim = atoi(($3).c_str()); 
};

udim: T_FZY_UDIM T_FZY_ASSIGN T_FZY_INT T_FZY_NEWLINE {
  udim = atoi(($3).c_str()); 
};

cdim: T_FZY_CDIM T_FZY_ASSIGN T_FZY_INT T_FZY_NEWLINE {
  cdim = atoi(($3).c_str()); 
};

sdim: T_FZY_SDIM T_FZY_ASSIGN T_FZY_INT T_FZY_NEWLINE {
  sdim = atoi(($3).c_str()); 
};

worst_rule: T_FZY_WORST_RULE T_FZY_ASSIGN T_FZY_INT T_FZY_NEWLINE {
  worst_rule = atoi(($3).c_str()); 
};


history: T_FZY_HISTORY T_FZY_ASSIGN T_FZY_VECTOR T_FZY_NEWLINE {
  size_t i = 0;
  do {
    if ( i < sdim ) {
      history[i] = atoi($3.c_str());
      i++;
    }
    else {
      return fzy_error("history vector too long");
    }
    stringpos = $3.find(' ') + 1;
    $3 = $3.substr(stringpos);
  } while (stringpos > 0);
  /* check history's completeness  */
  if (i != sdim) {
    return fzy_error("history vector too short");
  }
  Data d(udim, 1, 0.0);
  FModel firstmodel(d, cdim);
  ((FModel*)fmodel)->copy(firstmodel);
  /* build model using history */
  for (i = 0; i < sdim; i += 2) {
    FModel newmodel(*((FModel*)fmodel), history[i], history[i+1]);
    ((FModel*)fmodel)->copy(newmodel);
  }
} | T_FZY_HISTORY T_FZY_ASSIGN T_FZY_NULL T_FZY_NEWLINE {
  if (rdim > 1) {
    return fzy_error("history vector too short: vector = (null)");
  }
  Data d(udim, 1, 0.0);
  FModel firstmodel(d, cdim);
  ((FModel*)fmodel)->copy(firstmodel);
};


learnfn: T_FZY_LEARNFILENAME T_FZY_ASSIGN T_FZY_STRING T_FZY_NEWLINE {
  ((FModel*)fmodel)->learnfilename() = $3;
};

validfn: T_FZY_VALIDATIONFILENAME T_FZY_ASSIGN T_FZY_STRING T_FZY_NEWLINE {
  ((FModel*)fmodel)->validationfilename() = $3;
};


fsets: T_FZY_SETS sets {
  /* check model completeness */
  if (fsetindex < sdim)
    return fzy_error("not enough fsets");  
};
sets: empty {
  ;
} | sets set {
  ;
} | sets T_FZY_NEWLINE {
  ;
};  
set: T_FZY_SET_BEGIN T_FZY_VECTOR T_FZY_NEWLINE {
  if ( fsetindex >= sdim ) {
    return fzy_error("too many fsets");
  }
  size_t i = 0;
  do {
    if ( i < 8 ) {
      fsetvect[i] = atof($2.c_str());
      i++;
    }
    else {
      return fzy_error("fset vector too long");
    }
    stringpos = $2.find(' ') + 1;
    $2 = $2.substr(stringpos);
  } while (stringpos > 0);
  /* check completeness */
  if ( i != 8 ) {
    return fzy_error("fset vector too short");
  }
  /* copy values */
  i = 0;
  (((FModel*)fmodel)->fsets(fsetindex)).set_mu(fsetvect[i++]);
  (((FModel*)fmodel)->fsets(fsetindex)).set_sigma(fsetvect[i++]);
  (((FModel*)fmodel)->fsets(fsetindex)).d_mu() = fsetvect[i++];
  (((FModel*)fmodel)->fsets(fsetindex)).d_sigma() = fsetvect[i++];
  (((FModel*)fmodel)->fsets(fsetindex)).d_old_mu() = fsetvect[i++];
  (((FModel*)fmodel)->fsets(fsetindex)).d_old_sigma() = fsetvect[i++];
  (((FModel*)fmodel)->fsets(fsetindex)).set_delta_mu(fsetvect[i++]);
  (((FModel*)fmodel)->fsets(fsetindex)).set_delta_sigma(fsetvect[i++]);
  ++fsetindex;
} | T_FZY_NULL T_FZY_NEWLINE {
  ;
};


fcons: T_FZY_CONS consequences {
  /* check model completeness */
  if (consindex < rdim)
    return fzy_error("not enough consequences");
};
consequences: empty {
  ;
} | consequences consequence {
  ;
} | consequences T_FZY_NEWLINE {
  ;
};  
consequence: T_FZY_CONS_BEGIN T_FZY_VECTOR T_FZY_NEWLINE {
  if ( consindex >= rdim ) {
    return fzy_error("too many consequences");
  }
  size_t i = 0;
  do {
    if ( i < 4*cdim ) {
      consvect[i] = atof($2.c_str());
      i++;
    }
    else {
      return fzy_error("consequence vector too long");
    }
    stringpos = $2.find(' ') + 1;
    $2 = $2.substr(stringpos);
  } while (stringpos > 0);
  /* check completeness */
  if ( i != 4*cdim ) {
    return fzy_error("consequence vector too short");
  }
  /* copy values */
  Consequence::iterator pcons, pd_cons, pd_old_cons, pdelta_cons;
  pcons = (((FModel*)fmodel)->frules(consindex)).cons().begin();
  pd_cons = (((FModel*)fmodel)->frules(consindex)).d_cons().begin();
  pd_old_cons = (((FModel*)fmodel)->frules(consindex)).d_old_cons().begin();
  pdelta_cons = (((FModel*)fmodel)->frules(consindex)).delta_cons().begin();
  vector<Real>::const_iterator pconsvect = consvect.begin();
  for (i = 0; i < cdim; ++i) {
    *(pcons++) = *(pconsvect++);
  }
  for (i = 0; i < cdim; ++i) {
    *(pd_cons++) = *(pconsvect++);
  }
  for (i = 0; i < cdim; ++i) {
    *(pd_old_cons++) = *(pconsvect++);
  }
  for (i = 0; i < cdim; ++i) {
    *(pdelta_cons++) = *(pconsvect++);
  }
  ++consindex;
};


frules: T_FZY_RULES rules {
  /* check model completeness */
  if (ruleindex < rdim)
    return fzy_error("not enough rules");
};
rules: empty {
  ;
} | rules T_FZY_NEWLINE {
  ;
} | rules rule {
  ;
};  
rule: T_FZY_RULE_BEGIN {
  // PRINT($1);
  if (ruleindex >= rdim) {
    return fzy_error("too many rules");
  }
  ++ruleindex;
};


empty: /*** empty ***/
;


%% /****************** additional C code ************************/


/* called by fzy_parse() on error */
//#WIN2017 int fzy_error(char *errmsg)
int  fzy_error(const string& errmsg)
{
  throw ParseError(fzy_lineno, errmsg, fzy_lval);
  return 1;
}

