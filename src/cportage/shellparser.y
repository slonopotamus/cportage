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
