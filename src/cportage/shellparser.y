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

%pure-parser
%name-prefix="cportage_shellconfig_"
%locations
%defines
%error-verbose
%parse-param { cportage_shellconfig_context * context }
%lex-param { void * scanner }

%{
  #include "cportage/io.h"

  #define scanner context->yyscanner

  typedef struct cportage_shellconfig_context_t {
    void * yyscanner;
  } cportage_shellconfig_context;
%}

%union{
  int integer;
  char * string;
}

%token VARIABLE

%{
  void cportage_shellconfig_error(const YYLTYPE * locp, cportage_shellconfig_context * context, const char * err);

  int cportage_shellconfig_lex(YYSTYPE * yylval_param, YYLTYPE * yylloc_param, void * yyscanner);

  int cportage_shellconfig_lex_init(void ** yyscanner);

  int cportage_shellconfig_lex_destroy(void * yyscanner);

  void cportage_shellconfig_set_extra(cportage_shellconfig_context * ctx, void * yyscanner);
%}

%%

start:config;

config: /* empty */ | config decl;

decl: VARIABLE "\n";

%%

void cportage_shellconfig_error(const YYLTYPE * locp, cportage_shellconfig_context * context, const char * err) {
}

int cportage_read_shellconfig(const char * path, const bool allow_source, hash ** into) {
  cportage_shellconfig_context ctx;
  cportage_shellconfig_lex_init(&ctx.yyscanner);
  cportage_shellconfig_set_extra(&ctx, ctx.yyscanner);
  cportage_shellconfig_parse(ctx.yyscanner);
  cportage_shellconfig_lex_destroy(ctx.yyscanner);
  return 0;
}
