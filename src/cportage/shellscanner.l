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
%option batch bison-bridge bison-locations reentrant yylineno warn debug 8bit
%option nodefault noinput nounput noyywrap noyyget_extra noyy_push_state
%option noyy_pop_state noyy_top_state
%option noyyget_extra noyyget_leng noyyget_text noyyget_lineno noyyset_lineno
%option noyyget_in noyyget_out noyyset_out noyyget_lval noyyset_lval
%option noyyget_lloc noyyset_lloc noyyget_debug

%{

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wmissing-noreturn"
#pragma GCC diagnostic ignored "-Wmissing-prototypes"
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#pragma GCC diagnostic ignored "-Wunreachable-code"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-variable"

#include "shellparser.h"
#include "shellconfig_ctx.h"

#define YY_USER_ACTION yylloc->first_line = yylloc->last_line = yylineno; \
    yylloc->first_column = yycolumn; yylloc->last_column = yycolumn+yyleng; \
    yycolumn += yyleng;
#define YY_EXTRA_TYPE cp_shellconfig_ctx *
#define DUPSTR yylval->str = g_strdup(yytext)

%}

%s QUOTE_STATE SQUOTE_STATE

NewLine    \r?\n

%%

%{
    if (yyextra->magic) {
        int retval = yyextra->magic;
        yyextra->magic = 0;
        return retval;
    }
%}

<INITIAL>[[:blank:]]+#.* { /* comments */                                 }
<INITIAL>^#.*            { /* comments */                                 }
<INITIAL>[[:blank:]]+$   { /* trailing whitespace */                      }

\\{NewLine}              { /* line continuation */                        }
export                   {                              return EXPORT;    }
source                   {                              return SOURCE;    }
{NewLine}                { yylval->str = g_strdup(" "); return EOL;       }
[[:blank:]]+             { yylval->str = g_strdup(" "); return BLANK;     }
[a-zA-Z]+                { DUPSTR;                      return ALPHA;     }
[[:digit:]]+             { DUPSTR;                      return NUMBER;    }
\\([0-7]{2,3}|x[0-9a-fA-F]{1,2}|c.|.) { yylval->str = g_strcompress(yytext); return ESC_CHAR; }
\$                       { DUPSTR;                      return DOLLAR;    }
\.                       { DUPSTR;                      return DOT;       }
\=                       { DUPSTR;                      return EQUALS;    }
\_                       { DUPSTR;                      return UNDERLINE; }
\{                       { DUPSTR;                      return LBRACE;    }
\}                       { DUPSTR;                      return RBRACE;    }

<INITIAL>\"              { DUPSTR; BEGIN QUOTE_STATE;   return QUOTE;     }
<QUOTE_STATE>\"          { DUPSTR; BEGIN INITIAL;       return QUOTE;     }
\"                       { DUPSTR;                      return QUOTE;     }

<INITIAL>\'              { DUPSTR; BEGIN SQUOTE_STATE;  return SQUOTE;    }
<SQUOTE_STATE>\'         { DUPSTR; BEGIN INITIAL;       return SQUOTE;    }
\'                       { DUPSTR;                      return SQUOTE;    }

\#                       { DUPSTR;                      return POUND;     }
[&<>()|;`]               { DUPSTR;                      return QCHAR;     }
.                        { DUPSTR;                      return NQCHAR;    }

%%
