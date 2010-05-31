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
%name-prefix "cportage_shellconfig_"
%parse-param { cportage_shellconfig_ctx *ctx }
%verbose

%{

#include <glib/gi18n-lib.h>
#include <stdio.h>

#include "cportage/io.h"

#include "shellparser.h"
#include "shellscanner.h"

#define scanner ctx->yyscanner
#define YY_ _
#define YYSTACK_USE_ALLOCA 1

/* TODO: guard pragmas with GCC ifdef */
#pragma GCC diagnostic ignored "-Wstack-protector"
#pragma GCC diagnostic ignored "-Wunreachable-code"
#pragma GCC diagnostic ignored "-Wmissing-noreturn"

typedef struct cportage_shellconfig_ctx_t {
    const char *filename;
    GHashTable *entries;
    GError **error;
    bool allow_source;
    yyscan_t yyscanner;
} cportage_shellconfig_ctx;

%}

%union{
    char *str;
}

%destructor { g_free($$); }         <str>
%printer    { g_print("%s", $$); }  <str>

%{

static void
cportage_shellconfig_error(
    const YYLTYPE *locp G_GNUC_UNUSED,
    cportage_shellconfig_ctx *ctx G_GNUC_UNUSED,
    const char *err G_GNUC_UNUSED
) {
    /* TODO: set *(ctx->error) */
    if (locp->first_line) {
        g_print("%s: %d.%d-%d.%d: ", ctx->filename,
            locp->first_line, locp->first_column,
            locp->last_line, locp->last_column);
    }
    g_print("%s\n", err);
}

static bool
dosource(cportage_shellconfig_ctx *ctx, const char *path) {
    /* TODO: resolve relative path using realpath(ctx->filename) as base */
    /* TODO: protect against include loop? */
    return cportage_read_shellconfig(ctx->entries, path, true, ctx->error);
}

static char *
dolookup(const cportage_shellconfig_ctx *ctx, const char *key) {
    const char *found = g_hash_table_lookup(ctx->entries, key);
    return g_strdup(found == NULL ? "" : found);
}

%}

%token  <str> VAR STRING
%token        SOURCE
%type   <str> value value_item

%%

start:
  /* empty */
  | config

/*
  Any number of config items separated by one or more newlines.
  Credits to Timofey Basanov a.k.a. ripos for this.
 */
config:
    config_elem
    | '\n'
    | '\n' config_elem
    | config '\n'
    | config '\n' config_elem;

config_elem:
    /* TODO: handle stacking */
    VAR '=' '"' value '"' { g_hash_table_replace(ctx->entries, $1, $4); }
    | SOURCE STRING { if (!dosource(ctx, $2)) { YYERROR; } g_free($2); }

value:
    /* empty */         { $$ = g_strdup(""); }
    | value value_item  { $$ = g_strconcat($1, $2, NULL); g_free($1); g_free($2); } ;

value_item:
    STRING
    | '$' VAR         { $$ = dolookup(ctx, $2); g_free($2); }
    | '$' '{' VAR '}' { $$ = dolookup(ctx, $3); g_free($3); }

%%

bool
cportage_read_shellconfig(
    GHashTable *into,
    const char *path,
    bool allow_source,
    GError **error
) {
    cportage_shellconfig_ctx ctx;
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
        return FALSE;
    }

    ctx.filename = path;
    ctx.allow_source = allow_source;
    ctx.entries = into;
    ctx.error = error;

    cportage_shellconfig_lex_init(&ctx.yyscanner);
    cportage_shellconfig_set_extra(&ctx, ctx.yyscanner);
    cportage_shellconfig_set_in(f, ctx.yyscanner);
    cportage_shellconfig_set_debug(1, ctx.yyscanner);

    retval = cportage_shellconfig_parse(&ctx) == 0;

    cportage_shellconfig_lex_destroy(ctx.yyscanner);
    fclose(f);

    return retval;
}
