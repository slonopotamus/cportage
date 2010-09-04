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

typedef struct cp_shellconfig_ctx_t {
    yyscan_t yyscanner;
    const char *filename;
    GHashTable *entries;
    GError **error;
    bool allow_source;
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

static bool
dosource(cp_shellconfig_ctx *ctx, const char *path) {
    /* TODO: protect against include loop? */
    char *full;
    bool result;

    if (g_path_is_absolute(path)) {
        full = g_strdup(path);
    } else {
        char *dirname = g_path_get_dirname(ctx->filename);
        full = g_build_filename(dirname, path, NULL);
        g_free(dirname);
    }
    result = cp_read_shellconfig(ctx->entries, full, true, ctx->error);
    g_free(full);
    return result;
}

static char *
dolookup(const cp_shellconfig_ctx *ctx, const char *key) {
    const char *found = g_hash_table_lookup(ctx->entries, key);
    return g_strdup(found == NULL ? "" : found);
}

%}

%token <str> ALPHA
%token <str> NUMBER
%token SOURCE
%token EXPORT
%token SPACE "whitespace"
%token EOL "newline"
%type  <str> quot_val value value_item fname fname_part name name_end

%%

start:
    /* empty */
  | stmt_list

/*
  Any number of config items separated by one or more newlines.
  Credits to Timofey Basanov a.k.a. ripos for this.
 */
stmt_list:
    stmt
  | EOL
  | EOL stmt
  | stmt_list EOL
  | stmt_list EOL stmt

stmt:
    source_stmt
  | decl_stmt

source_stmt:
    source_op SPACE fname { if (!dosource(ctx, $3)) { YYERROR; } g_free($3); }

source_op:
    '.'
  | SOURCE

decl_stmt:
    export_op name '=' quot_val { g_hash_table_replace(ctx->entries, $2, $4); }

export_op:
    /* empty */
  | EXPORT SPACE

quot_val:
          value
  | '"'  value '"'  { $$ = $2; }
  | '\'' value '\'' { $$ = $2; }

value:
    /* empty */      { $$ = g_strdup(""); }
  | value value_item { $$ = g_strconcat($1, $2, NULL); g_free($1); g_free($2); }

value_item:
    fname
  | SPACE                     { $$ = g_strdup(" "); }
  | '$' name               { $$ = dolookup(ctx, $2); g_free($2); }
  | '$' '{' name '}' { $$ = dolookup(ctx, $3); g_free($3); }

fname:
    fname_part
  | fname fname_part { $$ = g_strconcat($1, $2, NULL); g_free($1); g_free($2); }

fname_part:
    ALPHA
  | NUMBER
  | SOURCE { $$ = g_strdup("source"); }
  | '_' { $$ = g_strdup("_"); }
  | '-' { $$ = g_strdup("-"); }
  | '.' { $$ = g_strdup("."); }
  | '/' { $$ = g_strdup("/"); }
  | '|' { $$ = g_strdup("|"); }
  | '%' { $$ = g_strdup("%"); }
  | '=' { $$ = g_strdup("="); }
  | '*' { $$ = g_strdup("*"); }
  | ':' { $$ = g_strdup(":"); }
  | '@' { $$ = g_strdup("@"); }
  | '{' { $$ = g_strdup("{"); }
  | '}' { $$ = g_strdup("}"); }
  | '\\' '$' { $$ = g_strdup("$"); }
  | '\\' '"' { $$ = g_strdup("\""); }

name:
    ALPHA name_end { $$ = g_strconcat($1, $2, NULL); g_free($1); g_free($2); }
  | '_' name_end   { $$ = g_strconcat("_", $2, NULL); g_free($2); }

name_end:
    /* empty */       { $$ = g_strdup(""); }
  | name_end '_'      { $$ = g_strconcat($1, "_", NULL); g_free($1); }
  | name_end ALPHA    { $$ = g_strconcat($1, $2, NULL); g_free($1); g_free($2); }
  | name_end NUMBER   { $$ = g_strconcat($1, $2, NULL); g_free($1); g_free($2); }

%%

GQuark
cp_shellconfig_error_quark(void) {
  return g_quark_from_static_string("cp-shellconfig-error-quark");
}

bool
cp_read_shellconfig(
    GHashTable *into,
    const char *path,
    bool allow_source,
    GError **error
) {
    cp_shellconfig_ctx ctx;
    bool retval;
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
        return false;
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
