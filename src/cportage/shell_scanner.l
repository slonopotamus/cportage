/*
    Copyright 2009-2014, Marat Radchenko

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

%option prefix="cp_shell_parser_"
%option batch bison-bridge bison-locations reentrant yylineno warn debug 8bit
%option nodefault noinput nounput noyywrap noyyget_extra noyy_push_state
%option noyy_pop_state noyy_top_state never-interactive
%option noyyget_extra noyyget_leng noyyget_text noyyget_lineno noyyset_lineno
%option noyyget_in noyyget_out noyyset_out noyyget_lval noyyset_lval
%option noyyget_lloc noyyset_lloc noyyget_debug

%{

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wtype-limits"

#include "shell_parser_ctx.h"
#include "shell_parser.h"

#define YY_USER_ACTION yylloc->first_line = yylloc->last_line = yylineno; \
    yylloc->first_column = yycolumn; yylloc->last_column = yycolumn+yyleng; \
    yycolumn += yyleng;
#define YY_EXTRA_TYPE cp_shell_parser_ctx *
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

<INITIAL>[[:blank:]]+#.* { /* comments */                                }
<INITIAL>^#.*            { /* comments */                                }
<INITIAL>[[:blank:]]+$   { /* trailing whitespace */                     }

\\{NewLine}              { /* line continuation */                       }
export                   {                             return EXPORT;    }
source                   {                             return SOURCE;    }
{NewLine}                {                             return EOL;       }
[[:blank:]]+             {                             return BLANK;     }
[a-zA-Z]+                { DUPSTR;                     return ALPHA;     }
[[:digit:]]+             { DUPSTR;                     return NUMBER;    }
\\([0-7]{2,3}|x[0-9a-fA-F]{1,2}|c.|.) { yylval->str = g_strcompress(yytext); return ESC_CHAR; }
\$                       {                             return DOLLAR;    }
\.                       {                             return DOT;       }
\=                       {                             return EQUALS;    }
\_                       {                             return UNDERLINE; }
\{                       {                             return LBRACE;    }
\}                       {                             return RBRACE;    }

<INITIAL>\"              {         BEGIN QUOTE_STATE;  return QUOTE;     }
<QUOTE_STATE>\"          {         BEGIN INITIAL;      return QUOTE;     }
\"                       {                             return QUOTE;     }

<INITIAL>\'              {         BEGIN SQUOTE_STATE; return SQUOTE;    }
<SQUOTE_STATE>\'         {         BEGIN INITIAL;      return SQUOTE;    }
\'                       {                             return SQUOTE;    }

\#                       {                             return POUND;     }
[&<>()|;`]               { DUPSTR;                     return QCHAR;     }
.                        { DUPSTR;                     return NQCHAR;    }

%%
