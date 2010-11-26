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

NewLine    \r?\n

%%

%{
    if (yyextra->magic) {
        int retval = yyextra->magic;
        yyextra->magic = 0;
        return retval;
    }
%}

[[:blank:]]*#.*  { /* comments          */   }
\\{NewLine}      { /* line continuation */   }
export           {         return EXPORT;    }
source           {         return SOURCE;    }
{NewLine}        { yylval->str = g_strdup(" "); return EOL; }
[[:blank:]]+     { DUPSTR; return BLANK;     }
[a-zA-Z]+        { DUPSTR; return ALPHA;     }
[[:digit:]]+     { DUPSTR; return NUMBER;    }
\\([0-7]{2,3}|x[0-9a-fA-F]{1,2}|c.|.) { yylval->str = g_strcompress(yytext); return ESC_CHAR; }
\$               { DUPSTR; return DOLLAR;    }
\>\=             { DUPSTR; return GEQ;       }
\<\=             { DUPSTR; return LEQ;       }
\=               { DUPSTR; return EQUALS;    }
\<               { DUPSTR; return LESS_THAN; }
\>               { DUPSTR; return GREATER_THAN; }
\_               { DUPSTR; return UNDERLINE; }
\!               { DUPSTR; return BANG;      }
\,               { DUPSTR; return COMMA;     }
\.{2}            { DUPSTR; return DOTDOT;    }
\.               { DUPSTR; return DOT;       }
\{               { DUPSTR; return LBRACE;    }
\}               { DUPSTR; return RBRACE;    }
\({2}            { DUPSTR; return LLPAREN;   }
\(               { DUPSTR; return LPAREN;    }
\){2}            { DUPSTR; return RRPAREN;   }
\)               { DUPSTR; return RPAREN;    }
\[               { DUPSTR; return LSQUARE;   }
\]               { DUPSTR; return RSQUARE;   }
\~               { DUPSTR; return TILDE;     }
\`               { DUPSTR; return TICK;      }
\/               { DUPSTR; return SLASH;     }
\"               { DUPSTR; return QUOTE;     }
\'               { DUPSTR; return SQUOTE;    }
\+{2}            { DUPSTR; return INC;       }
\+               { DUPSTR; return PLUS;      }
\-{2}            { DUPSTR; return DEC;       }
\-               { DUPSTR; return MINUS;     }
\*               { DUPSTR; return TIMES;     }
\%{2}            { DUPSTR; return PCTPCT;    }
\%               { DUPSTR; return PCT;       }
\|{2}            { DUPSTR; return LOGICOR;   }
\|               { DUPSTR; return PIPE;      }
\&{2}            { DUPSTR; return LOGICAND;  }
\&               { DUPSTR; return AMP;       }
\^               { DUPSTR; return CARET;     }
\?               { DUPSTR; return QMARK;     }
\:               { DUPSTR; return COLON;     }
\;{2}            { DUPSTR; return DOUBLE_SEMIC; }
\;               { DUPSTR; return SEMIC;     }
\@               { DUPSTR; return AT;        }
\#{2}            { DUPSTR; return POUNDPOUND; }
\#               { DUPSTR; return POUND;     }
.                { DUPSTR; return NQSTR;     }

%%
