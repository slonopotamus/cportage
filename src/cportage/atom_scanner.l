/*
    Copyright 2009-2011, Marat Radchenko

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

%option prefix="cp_atom_parser_"
%option batch bison-bridge reentrant warn debug 8bit
%option nodefault noinput nounput noyywrap noyyget_extra noyy_push_state
%option noyy_pop_state noyy_top_state never-interactive
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

#include "atom_parser_types.h"
#include "atom_parser_ctx.h"
#include "atom_parser.h"

#define YY_EXTRA_TYPE cp_atom_parser_ctx *
#define DUPSTR yylval->str = g_strdup(yytext)

%}

%%

%{
    if (yyextra->magic) {
        int retval = yyextra->magic;
        yyextra->magic = 0;
        return retval;
    }
%}

alpha        {         return ALPHA;     }
beta         {         return BETA;      }
pre          {         return PRE;       }
rc           {         return RC;        }
p            {         return P;         }
r            {         return R;         }
[a-z] { yylval->chr = yytext[0]; return LOWER; }
[A-Z]+       { DUPSTR; return UPPER;     }
[[:digit:]]+ { DUPSTR; return NUMBER;    }
\+           {         return PLUS;      }
\-           {         return MINUS;     }
\_           {         return UNDERLINE; }
\<           {         return LT;        }
\>           {         return GT;        }
\=           {         return EQ;        }
\~           {         return TILDE;     }
\*           {         return STAR;      }
\.           {         return DOT;       }
\:           {         return COLON;     }
\/           {         return SLASH;     }
\[           {         return LSQUARE;   }
\,           {         return COMMA;     }
\]           {         return RSQUARE;   }
\@           {         return AT;        }
\!           {         return EXCL;      }
\?           {         return QMARK;     }
.|\n         {         return yytext[0]; }

%%
