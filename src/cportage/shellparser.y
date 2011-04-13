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

#include "error.h"
#include "io.h"
#include "shellconfig.h"
#include "strings.h"

#include "shellparser.h"
#include "shellscanner.h"
#include "shellconfig_ctx.h"

#define scanner ctx->yyscanner
#define YYDEBUG 1
#define YY_ _

#define DOCONCAT2(str1, str2) g_strconcat(str1, str2, NULL); \
    g_free(str1); g_free(str2);
#define DOCONCAT3(str1, str2, str3) g_strconcat(str1, str2, str3, NULL); \
    g_free(str1); g_free(str2); g_free(str3);

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
        g_set_error(ctx->error, CP_ERROR, CP_ERROR_SHELLCONFIG_SYNTAX,
            _("Could not parse '%s' at %d.%d-%d.%d: %s"),
            ctx->filename,
            locp->first_line, locp->first_column,
            locp->last_line, locp->last_column,
            err);
    } else {
        g_set_error(ctx->error, CP_ERROR, CP_ERROR_SHELLCONFIG_SYNTAX,
            _("Could not parse %s: %s"), ctx->filename, err);
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

%token <str> ALPHA BLANK DOLLAR DOT EOL EQUALS ESC_CHAR LBRACE NQCHAR NUMBER
%token <str> POUND QCHAR QUOTE RBRACE SQUOTE UNDERLINE

%token EXPORT SOURCE VAR_MAGIC FILE_MAGIC

%type <str> blank value var_ref
%type <str> fname fname_part vname vname_start vname_end vname_end_part
%type <str> sqstr sqstr_loop sqstr_part
%type <str> dqstr dqstr_loop dqstr_part
%type <str> nqstr_part qstr_part

%%

start:
    FILE_MAGIC file
  | VAR_MAGIC dqstr_loop { ctx->expanded = $2; }

file:
    /* empty */
  | stmt_list

/*
  Any number of statements separated by one or more newlines.
  Credits to Timofey Basanov a.k.a. ripos for this.
 */
stmt_list:
    stmt_with_blank
  | EOL { g_free($1); }
  | EOL stmt_with_blank { g_free($1); }
  | stmt_list EOL { g_free($2); }
  | stmt_list EOL stmt_with_blank { g_free($2); }

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
    DOT { g_free($1); }
  | SOURCE

/* --- Variable definition --- */

var_def_stmt:
    var_def
  | EXPORT blank var_def { g_free($2); }

var_def:
    vname EQUALS value { g_hash_table_insert(ctx->entries, $1, $3); g_free($2); }

value:
    /* empty */ { $$ = g_strdup(""); }
  | fname

var_ref:
    DOLLAR        vname        { $$ = dolookup(ctx, $2); g_free($1); g_free($2); }
  | DOLLAR LBRACE vname RBRACE { $$ = dolookup(ctx, $3); g_free($1); g_free($2); g_free($3); g_free($4); }

/* --- Filename --- */

fname:
    fname_part
  | fname fname_part { $$ = DOCONCAT2($1, $2); }

fname_part:
    var_ref
  | sqstr
  | dqstr
  | nqstr_part

/* --- Variable name --- */

vname:
    vname_start vname_end { $$ = DOCONCAT2($1, $2); }

vname_start:
    UNDERLINE
  | SOURCE    { $$ = g_strdup("source"); }
  | EXPORT    { $$ = g_strdup("export"); }
  | ALPHA

vname_end:
    /* empty */              { $$ = g_strdup(""); }
  | vname_end vname_end_part { $$ = DOCONCAT2($1, $2); }

vname_end_part:
    vname_start
  | NUMBER

/* -- Double-quoted string -- */
dqstr:
    QUOTE dqstr_loop QUOTE { $$ = $2; g_free($1); g_free($3); }

dqstr_loop:
    /* empty */           { $$ = g_strdup(""); }
  | dqstr_loop dqstr_part { $$ = DOCONCAT2($1, $2); }

dqstr_part:
    qstr_part
  | var_ref
  | SQUOTE

/* -- Single-quoted string -- */
sqstr:
    SQUOTE sqstr_loop SQUOTE { $$ = $2; g_free($1); g_free($3); }

sqstr_loop:
    /* empty */           { $$ = g_strdup(""); }
  | sqstr_loop sqstr_part { $$ = DOCONCAT2($1, $2); }

sqstr_part:
    qstr_part
  | DOLLAR
  | QUOTE

/* -- Misc string parts -- */

/* Symbols that can be part of quoted string */
qstr_part:
    nqstr_part
  | QCHAR
  | BLANK
  | EOL

/* Symbols that can be part of nonquoted string */
nqstr_part:
    vname_end_part
  | NQCHAR
  | DOT
  | EQUALS
  | ESC_CHAR
  | POUND
  | LBRACE
  | RBRACE

blank:
    BLANK
  | blank BLANK { $$ = DOCONCAT2($1, $2); }

%%

static gboolean
doparse(
    cp_shellconfig_ctx *ctx,
    GHashTable *entries,
    const char *path,
    gboolean allow_source,
    int magic,
    GError **error
) {
    g_assert(error == NULL || *error == NULL);

    ctx->filename = path;
    ctx->entries = entries;
    ctx->error = error;
    ctx->allow_source = allow_source;
    ctx->magic = magic;
    cp_shellconfig_set_extra(ctx, ctx->yyscanner);

    if (cp_string_is_true(g_getenv("CPORTAGE_SHELLCONFIG_DEBUG"))) {
        cp_shellconfig_debug = 1;
        cp_shellconfig_set_debug(1, ctx->yyscanner);
    }

    return cp_shellconfig_parse(ctx) == 0;
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

    f = cp_io_fopen(path, "r", error);
    if (f == NULL) {
        return FALSE;
    }

    cp_shellconfig_lex_init(&ctx.yyscanner);
    cp_shellconfig_set_in(f, ctx.yyscanner);
    retval = doparse(&ctx, into, path, allow_source, FILE_MAGIC, error);
    cp_shellconfig_lex_destroy(ctx.yyscanner);
    fclose(f);

    return retval;
}

char *
cp_varexpand(const char *str, GHashTable *vars G_GNUC_UNUSED, GError **error) {
    cp_shellconfig_ctx ctx;
    YY_BUFFER_STATE bp;

    g_assert(error == NULL || *error == NULL);

    ctx.expanded = NULL;

    cp_shellconfig_lex_init(&ctx.yyscanner);
    bp = cp_shellconfig__scan_string(str, ctx.yyscanner);
    cp_shellconfig__switch_to_buffer(bp, ctx.yyscanner);
    if (doparse(&ctx, vars, str, FALSE, VAR_MAGIC, error)) {
        g_assert(ctx.expanded != NULL);
    } else {
        g_free(ctx.expanded);
        ctx.expanded = NULL;
    }
    cp_shellconfig__delete_buffer(bp, ctx.yyscanner);
    cp_shellconfig_lex_destroy(ctx.yyscanner);

    return ctx.expanded;
}
