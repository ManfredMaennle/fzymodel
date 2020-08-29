
/*
 * data_lex.l -- a lexical analyzer for data
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
 * $Id: data_lex.l,v 2.9 2020-08-29 10:47:35+00:00:02 manfred Exp $
 *
 */

%{ /**************** C++ declarations *************************************/


#include "data_lex.h" 


/* local declarations for this file */

/* none */

%} /**************** analyzer declarations *********************************/

%option c++
%option prefix="data_"
%option noyywrap
%option nounput
%option yylineno


string  \"[^\n\"]*\"

ws      [ \t]+

alpha   [A-Za-z]
dig     [0-9]
name    ({alpha}|{dig}|\$)({alpha}|{dig}|[_.\-/$])*
int     [-+]?{dig}+
num1    [-+]?{dig}+\.?([eE][-+]?{dig}+)?
num2    [-+]?{dig}*\.{dig}+([eE][-+]?{dig}+)?
number  {int}|{num1}|{num2}


%% /**************** analyzer rules and actions ****************************/

{ws}    /* skip blanks and tabs */

{number}  return T_DATA_NUMBER;
   
\n        return T_DATA_NEWLINE;

<<EOF>>   return T_DATA_EOF;

 
##shift:  return T_DATA_SCALE_SHIFT;

##factor: return T_DATA_SCALE_FACTOR;

##comment:.* return T_DATA_COMMENT;
##source:.* return T_DATA_COMMENT;
##dynamics:.* return T_DATA_COMMENT;
##rows:.* return T_DATA_COMMENT;
##columns:.* return T_DATA_COMMENT;
##names:.* return T_DATA_COMMENT;

#\ .*     return T_DATA_COMMENT;

{name}    return T_DATA_ERROR;
   
{string}  return T_DATA_ERROR; /* cout << "string " << YYText() << '\n'; */
   
.         return T_DATA_ERROR; 
          /* avoid warnings at -Wall */ //#WIN2017 yy_flex_realloc(NULL,0);goto find_rule;


%% /**************** additional C code ************************************/

/* none */

/* ##.*      return T_DATA_COMMENT; */

