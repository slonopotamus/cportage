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
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#pragma GCC diagnostic ignored "-Wunreachable-code"
#pragma GCC diagnostic ignored "-Wunused-parameter"

#include <glib.h>

#include "shellparser.h"

#define YY_USER_ACTION yylloc->first_line = yylloc->last_line = yylineno; \
    yylloc->first_column = yycolumn; yylloc->last_column = yycolumn+yyleng; \
    yycolumn += yyleng;
#define YY_EXTRA_TYPE cp_shellconfig_ctx *
#define DUPSTR yylval->str = g_strdup(yytext)

typedef struct cp_shellconfig_ctx_t cp_shellconfig_ctx;

%}

NewLine    \r?\n

%%

[[:blank:]]*#.*  { /* comments          */   }
\\{NewLine}      { /* line continuation */   }
export           {         return EXPORT;    }
source           {         return SOURCE;    }
{NewLine}        {         return EOL;       }
[[:blank:]]+     { DUPSTR; return BLANK;     }
[a-zA-Z]+        { DUPSTR; return ALPHA;     }
[[:digit:]]+     { DUPSTR; return NUMBER;    }
\\([0-7]{2,3}|x[0-9a-fA-F]{1,2}|c.|.) { DUPSTR; return ESC_CHAR; }
.                {         return yytext[0]; }

%%
