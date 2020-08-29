
/*
 * fzy_lex.l -- a lexical analyzer for fuzzy models
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
 * $Id: fzy_lex.l,v 2.9 2020-08-29 10:47:37+00:00:02 manfred Exp $
 *
 */

%{ /**************** C++ declarations *************************************/


#ifndef WIN2017
#include <fstream.h>
#else
// #include <string>
#include <fstream>
#endif


#include "fzy_lex.h" 
#include "fzy_prs.h"



/* local declarations for this file */



%} /**************** analyzer declarations *********************************/

%option prefix="fzy_"
%option noyywrap
%option nounput
%option yylineno


string  \"[^\n\"]*\"

ws      [ \t]+

alpha   [A-Za-z]
dig     [0-9]
int     [-+]?{dig}+
num1    [-+]?{dig}+\.?([eE][-+]?{dig}+)?
num2    [-+]?{dig}*\.{dig}+([eE][-+]?{dig}+)?
number  {int}|{num1}|{num2}
name    {alpha}({alpha}|{dig}|_)*

%% /**************** analyzer rules and actions ****************************/

{ws}      /* skip blanks and tabs */

#.*\n     /* skip comments */

{int}     fzy_lval = yytext; return T_FZY_INT;

{number}  fzy_lval = yytext; return T_FZY_NUMBER;
   
({number}{ws})+({number}) fzy_lval = yytext; return T_FZY_VECTOR;

{string}  { 
fzy_lval = ((string)yytext).substr(1, strlen(yytext)-2); 
return T_FZY_STRING;
}
   
\n        return T_FZY_NEWLINE;

<<EOF>>   return T_FZY_EOF;

"="       return T_FZY_ASSIGN;

"rdim"    return T_FZY_RDIM;

"udim"    return T_FZY_UDIM;

"cdim"    return T_FZY_CDIM;

"sdim"    return T_FZY_SDIM;

"worst_rule" return T_FZY_WORST_RULE;

"learnfilename" return T_FZY_LEARNFILENAME;

"validationfilename" return T_FZY_VALIDATIONFILENAME;

"history" return T_FZY_HISTORY;

"fsets:\n" return T_FZY_SETS;

"consequences:\n" return T_FZY_CONS;

"frules:\n" return T_FZY_RULES;

"(null)" return T_FZY_NULL;

"F"{dig}+":" return T_FZY_SET_BEGIN;

"C"{dig}+":" return T_FZY_CONS_BEGIN;

"R"{dig}+":".*"\n" fzy_lval = yytext; return T_FZY_RULE_BEGIN;

{name}    fzy_lval = yytext; return T_FZY_ERROR;
          /* cerr << "[string " << yytext << "]\n";*/

.         fzy_lval = yytext; return T_FZY_ERROR; 
          /* avoid warnings at -Wall */ //#WIN2017 yy_flex_realloc(NULL,0);goto find_rule;
          


%% /**************** additional C code ************************************/

/* none */
