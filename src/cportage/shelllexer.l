%option reentrant
%option prefix="cportage_shellconfig_"
%option bison-bridge
%option bison-locations
%option yylineno
%option noyywrap
%option noinput
%option nounput
%option batch

%{
  #include "shellparser.h"

  #define YY_USER_ACTION yylloc->first_line = yylineno;
  #define YY_EXTRA_TYPE cportage_shellconfig_context *

  typedef struct cportage_shellconfig_context_t cportage_shellconfig_context;
%}

%%

[a-zA-Z][a-zA-Z0-9_-]* return VARIABLE;
#.* /* ignore comments */;
\n  /* ignore newlines */ ;
.   ;

%%
