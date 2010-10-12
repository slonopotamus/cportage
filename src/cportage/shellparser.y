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

/* Parts of this grammar were borrowed from libbash project */

%define api.pure
%defines
%error-verbose
%lex-param { void *scanner }
%locations
%name-prefix "cp_shellconfig_"
%parse-param { cp_shellconfig_ctx *ctx }
%verbose

%{

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wlogical-op"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wstrict-overflow"
#pragma GCC diagnostic ignored "-Wunreachable-code"

#include "cportage/io.h"
#include "cportage/shellconfig.h"
#include "cportage/strings.h"

#include "shellparser.h"
#include "shellscanner.h"

#define scanner ctx->yyscanner
#define YYDEBUG 1
#define YY_ _

#define DOCONCAT2(str1, str2) g_strconcat(str1, str2, NULL); \
    g_free(str1); g_free(str2);
#define DOCONCAT3(str1, str2, str3) g_strconcat(str1, str2, str3, NULL); \
    g_free(str1); g_free(str2); g_free(str3);

typedef struct cp_shellconfig_ctx_t {
    yyscan_t yyscanner;
    const char *filename;
    GHashTable *entries;
    GError **error;
    gboolean allow_source;
} cp_shellconfig_ctx;

%}

%union{
    char *str;
}

%destructor { g_free($$); }         <str>
%printer    { g_print("%s", $$); }  <str>

%{

static void
cp_shellconfig_error(
    const YYLTYPE *locp,
    cp_shellconfig_ctx *ctx,
    const char *err
) {
    if (ctx->error == NULL || *ctx->error != NULL) {
        return;
    }

    if (locp->first_line) {
        g_set_error(ctx->error,
            CP_SHELLCONFIG_ERROR,
            CP_SHELLCONFIG_ERROR_SYNTAX,
            _("Could not parse '%s' at %d.%d-%d.%d: %s"),
            ctx->filename,
            locp->first_line, locp->first_column,
            locp->last_line, locp->last_column,
            err
        );
    } else {
        g_set_error(ctx->error,
            CP_SHELLCONFIG_ERROR,
            CP_SHELLCONFIG_ERROR_SYNTAX,
            _("Could not parse %s: %s"),
            ctx->filename,
            err
        );
    }
}

static gboolean
dosource(cp_shellconfig_ctx *ctx, const char *path) {
    /* TODO: protect against include loop? */
    char *full;
    gboolean result;

    if (g_path_is_absolute(path)) {
        full = g_strdup(path);
    } else {
        char *dirname = g_path_get_dirname(ctx->filename);
        full = g_build_filename(dirname, path, NULL);
        g_free(dirname);
    }
    result = cp_read_shellconfig(ctx->entries, full, TRUE, ctx->error);

    g_free(full);
    return result;
}

static char *
dolookup(const cp_shellconfig_ctx *ctx, const char *key) {
    const char *found = g_hash_table_lookup(ctx->entries, key);
    return g_strdup(found == NULL ? "" : found);
}

%}

%token <str> ALPHA NUMBER NQSTR ESC_CHAR
%token <str> BLANK "whitespace"
%token SOURCE
%token EXPORT
%token EOL "newline"
%type <str> blank value var_ref
%type <str> fname fname_part vname vname_start vname_end vname_end_part
%type <str> sqstr sqstr_loop sq_str_part
%type <str> dqstr dqstr_loop dq_str_part dqstr_part
%type <str> ns_str_part ns_str_part_no_res str_part str_part_with_pound str_part_with_pound_loop
%type <str> pattern_match_trigger

%%

start:
    /* empty */
  | stmt_list

/*
  Any number of statements separated by one or more newlines.
  Credits to Timofey Basanov a.k.a. ripos for this.
 */
stmt_list:
    stmt
  | EOL
  | EOL stmt_with_blank
  | stmt_list EOL
  | stmt_list EOL stmt_with_blank

/* Statement, optionally surrounded with blanks */
stmt_with_blank:
    stmt
  | blank stmt { g_free($1); }
  | stmt blank { g_free($2); }
  | blank stmt blank { g_free($1); g_free($3); }

/* Possible statements */
stmt:
    source_stmt
  | var_def_stmt

/* --- Sourcing --- */

source_stmt:
    source_op blank fname
        { if (!dosource(ctx, $3)) { YYABORT; /* TODO: memory leak of $2 and $3? */ } g_free($2); g_free($3); }

source_op:
    '.'
  | SOURCE

/* --- Variable definition --- */

var_def_stmt:
    var_def
  | EXPORT blank var_def { g_free($2); }

var_def:
    vname '=' value { g_hash_table_replace(ctx->entries, $1, $3); }

value:
    /* empty */ { $$ = g_strdup(""); }
  | fname

var_ref:
    '$'     vname     { $$ = dolookup(ctx, $2); g_free($2); }
  | '$' '{' vname '}' { $$ = dolookup(ctx, $3); g_free($3); }

/* --- Filename --- */

fname:
    fname_part
  | fname fname_part { $$ = DOCONCAT2($1, $2); }

fname_part:
    var_ref
  | sqstr
  | dqstr
  /* TODO: causes shift/reduce conflict because we have loop of loops of str_part */
  | str_part str_part_with_pound_loop { $$ = DOCONCAT2($1, $2); }

/* --- Variable name --- */

vname:
    vname_start vname_end { $$ = DOCONCAT2($1, $2); }

vname_start:
    '_'    { $$ = g_strdup("_"); }
  | SOURCE { $$ = g_strdup("source"); }
  | EXPORT { $$ = g_strdup("export"); }
  | ALPHA

vname_end:
    /* empty */ { $$ = g_strdup(""); }
  | vname_end vname_end_part { $$ = DOCONCAT2($1, $2); }

vname_end_part:
    vname_start
  | NUMBER

/* -- Double-quoted string -- */
dqstr:
    '"' dqstr_loop '"' { $$ = $2; }

dqstr_loop:
    /* empty */           { $$ = g_strdup(""); }
  | dqstr_loop dqstr_part { $$ = DOCONCAT2($1, $2); }

dqstr_part:
  /*  bracket_pattern_match
  |	extended_pattern_match
  |	*/var_ref
  /*|	command_sub
  |	arithmetic_expansion*/
  |	dq_str_part
  |	pattern_match_trigger
  |	'!' { $$ = g_strdup("!"); }

dq_str_part:
    str_part_with_pound
  | BLANK
  | EOL { $$ = g_strdup(" "); }
  | '{' { $$ = g_strdup("{"); }
  | '}' { $$ = g_strdup("}"); }
  | '|' { $$ = g_strdup("|"); }
  /* |BLANK|EOL|AMP|LOGICAND|LOGICOR|LESS_THAN|GREATER_THAN|PIPE|SQUOTE|SEMIC|COMMA|LPAREN|RPAREN|LLPAREN|RRPAREN|DOUBLE_SEMIC|LBRACE|RBRACE|TICK|LEQ|GEQ */

/* -- Single-quoted string -- */
sqstr:
    '\'' sqstr_loop '\'' { $$ = $2; }

sqstr_loop:
    /* empty */            { $$ = g_strdup(""); }
  | sqstr_loop sq_str_part { $$ = DOCONCAT2($1, $2); }

sq_str_part:
    str_part_with_pound
  | BLANK
  | '$' { $$ = g_strdup("$"); }
  | '{' { $$ = g_strdup("{"); }
  | '}' { $$ = g_strdup("}"); }
  | '|' { $$ = g_strdup("|"); }
  /* |BLANK|EOL|AMP|LOGICAND|LOGICOR|LESS_THAN|GREATER_THAN|PIPE|QUOTE|SEMIC|COMMA|LPAREN|RPAREN|LLPAREN|RRPAREN|DOUBLE_SEMIC|LBRACE|RBRACE|DOLLAR|TICK|BOP|UOP */

/* --Misc string parts -- */

str_part:
    ns_str_part
  |	'/' { $$ = g_strdup("/"); }

str_part_with_pound:
    str_part
  |	'#' { $$ = g_strdup("#"); }
  /* |	POUNDPOUND */

str_part_with_pound_loop:
    /* empty */ { $$ = g_strdup(""); }
  | str_part_with_pound_loop str_part_with_pound { $$ = DOCONCAT2($1, $2); }

ns_str_part:
    ns_str_part_no_res
/*  |	res_word_str

res_word_str:
  CASE|DO|DONE|ELIF|ELSE|ESAC|FI|FOR|FUNCTION|IF|IN|SELECT|THEN|UNTIL|WHILE|TIME
*/

ns_str_part_no_res:
    NUMBER
	|	vname
	| NQSTR
	| '=' { $$ = g_strdup("="); }
	| '.' { $$ = g_strdup("."); }
	| '-' { $$ = g_strdup("-"); }
	| ':' { $$ = g_strdup(":"); }
	| '%' { $$ = g_strdup("%"); }
	| ESC_CHAR
	/*|PCT|PCTPCT|MINUS|DOT|DOTDOT|COLON|BOP|UOP|TEST|'_'|TILDE|INC|DEC|ARITH_ASSIGN|ESC_CHAR|CARET*/

pattern_match_trigger:
    '[' { $$ = g_strdup("["); }
  |	']' { $$ = g_strdup("]"); }
  |	'?' { $$ = g_strdup("?"); }
  |	'+' { $$ = g_strdup("+"); }
  |	'*' { $$ = g_strdup("*"); }
  |	'@' { $$ = g_strdup("@"); }

blank:
    BLANK
  | blank BLANK { $$ = DOCONCAT2($1, $2); }

%%

GQuark
cp_shellconfig_error_quark(void) {
  return g_quark_from_static_string("cp-shellconfig-error-quark");
}

gboolean
cp_read_shellconfig(
    GHashTable *into,
    const char *path,
    gboolean allow_source,
    GError **error
) {
    cp_shellconfig_ctx ctx;
    gboolean retval;
    FILE *f;

    g_assert(error == NULL || *error == NULL);

    f = fopen(path, "r");
    if (f == NULL) {
        g_set_error(error,
            G_FILE_ERROR,
            g_file_error_from_errno(errno),
            _("Error reading file '%s': %s"),
            path,
            g_strerror(errno));
        return FALSE;
    }

    ctx.filename = path;
    ctx.entries = into;
    ctx.error = error;
    ctx.allow_source = allow_source;

    cp_shellconfig_lex_init(&ctx.yyscanner);
    cp_shellconfig_set_extra(&ctx, ctx.yyscanner);
    cp_shellconfig_set_in(f, ctx.yyscanner);

    if (cp_string_is_true(g_getenv("CPORTAGE_SHELLCONFIG_DEBUG"))) {
        cp_shellconfig_debug = 1;
        cp_shellconfig_set_debug(1, ctx.yyscanner);
    }

    retval = cp_shellconfig_parse(&ctx) == 0;

    cp_shellconfig_lex_destroy(ctx.yyscanner);
    fclose(f);

    return retval;
}
