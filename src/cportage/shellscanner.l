/*
    Copyright 2009-2010, Marat Radchenko

    This file is part of cportage.

    cportage is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    cportage is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with cportage.  If not, see <http://www.gnu.org/licenses/>.
*/

%option prefix="cp_shellconfig_"
%option batch bison-bridge bison-locations reentrant yylineno warn debug
%option nodefault noinput nounistd nounput noyywrap

%{

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wmissing-noreturn"
#pragma GCC diagnostic ignored "-Wmissing-prototypes"
#pragma GCC diagnostic ignored "-Wpadded"
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#pragma GCC diagnostic ignored "-Wunreachable-code"
#pragma GCC diagnostic ignored "-Wunused-parameter"

#include <glib.h>

#include "shellparser.h"

#define YY_USER_ACTION yylloc->first_line = yylloc->last_line = yylineno; \
    yylloc->first_column = yycolumn; yylloc->last_column = yycolumn+yyleng-1; \
    yycolumn += yyleng;
#define YY_EXTRA_TYPE cp_shellconfig_ctx *
#define DUPVAL(idx) yylval->str = g_strdup(&yytext[idx])

typedef struct cp_shellconfig_ctx_t cp_shellconfig_ctx;

%}

%x VALUE_STATE DOLLAR_STATE SOURCE_STATE

NewLine \r\n|\n|\r
VarName [a-zA-Z][a-zA-Z0-9_]*
Whitespace [ \t]+

%%

<INITIAL,SOURCE_STATE>{Whitespace} /* skip whitespace */
<INITIAL>#[^\r\n]*  /* skip comments */
<INITIAL>source { BEGIN SOURCE_STATE; return SOURCE; }
<INITIAL>{VarName} { DUPVAL(0); return VAR; }
<INITIAL>= { BEGIN VALUE_STATE; return yytext[0]; }

<INITIAL,VALUE_STATE,SOURCE_STATE>{NewLine} { BEGIN INITIAL; return '\n'; }
<INITIAL,VALUE_STATE>\\{NewLine} /* line continuation */

<VALUE_STATE>\\[$"] { DUPVAL(1); return STRING; }
<VALUE_STATE>[^\\\r\n$"]+ { DUPVAL(0); return STRING; }
<VALUE_STATE>\$ { BEGIN DOLLAR_STATE; return yytext[0]; }
<DOLLAR_STATE>\} { BEGIN VALUE_STATE; return yytext[0]; }
<DOLLAR_STATE>{VarName}/[^\}] { BEGIN VALUE_STATE; DUPVAL(0); return VAR; }
<DOLLAR_STATE>{VarName}/\} { DUPVAL(0); return VAR; }

<SOURCE_STATE>[^ \t\r\n$]+ { DUPVAL(0); BEGIN INITIAL; return PATH; }

<INITIAL,VALUE_STATE,DOLLAR_STATE,SOURCE_STATE>.|\n { return yytext[0]; }

%%
