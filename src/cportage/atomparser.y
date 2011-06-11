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

%define api.pure
%glr-parser
%defines
%expect 4
%lex-param { void *scanner }
%name-prefix "cp_atomparser_"
%parse-param { cp_atomparser_ctx *ctx }
%verbose

%{

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wmissing-noreturn"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-parameter"

/*
#pragma GCC diagnostic ignored "-Wlogical-op"
#pragma GCC diagnostic ignored "-Wstrict-overflow"
#pragma GCC diagnostic ignored "-Wunreachable-code"
*/

#include <gmp.h>

#include "atom.h"
#include "error.h"
#include "macros.h"
#include "strings.h"
#include "version.h"

#include "atomparser_types.h"
#include "atomparser.h"
#include "atomscanner.h"
#include "atomparser_ctx.h"

#define scanner ctx->yyscanner
#define YYDEBUG 1
#define YY_ _

#define DOCONCAT2(str1, str2) g_strconcat(str1, str2, NULL); \
    g_free(str1); g_free(str2);

static void
suffix_free(VersionSuffix suffix) {
    g_free(suffix->value);
    g_free(suffix);
}

%}

%union{
    char *str;
    GList/*<char *>*/ *str_list;
    GList/*<VersionSuffix>*/ *suffix_list;
    CPAtom atom;
    CPVersion version;
    struct pv pv;
    VersionSuffixType suffix_type;
    char chr;
}

%destructor { g_free($$);        } <str>
%printer    { g_print("%s", $$); } <str>

%destructor { g_list_free_full($$, g_free); } <str_list>

%destructor { cp_atom_unref($$);              } <atom>

%destructor { cp_version_unref($$);                } <version>
%printer    { g_print("%s", cp_version_str($$));   } <version>

%destructor { g_free($$.package); cp_version_unref($$.version);         } <pv>
%printer    { g_print("%s-%s", $$.package, cp_version_str($$.version)); } <pv>

%destructor { g_list_free_full($$, (GDestroyNotify)suffix_free); } <suffix_list>

%{

static void
cp_atomparser_error(
    cp_atomparser_ctx *ctx G_GNUC_UNUSED,
    const char *err G_GNUC_UNUSED
) {
    /* noop */
}

typedef enum OpType {
    OP_NONE,
    OP_LT,
    OP_LE,
    OP_EQ,
    OP_GE,
    OP_GT,
    OP_TILDE,
    OP_GLOB
} OpType;

struct CPAtom {
    /*@only@*/ char *category;
    /*@only@*/ char *package;
    /*@null@*/ CPVersion version;
    /*@null@*/ /*@only@*/ char *slot;
    /*@null@*/ /*@only@*/ char *repo;

    /*@refs@*/ unsigned int refs;
    OpType op;
};

static CPAtom
cp_atom_alloc(char *category, char *package, /*@null@*/ CPVersion version) {
    CPAtom result;

    result = g_new(struct CPAtom, 1);
    result->refs = 1;
    result->category = category;
    result->package = package;
    result->version = version;

    return result;
}

struct CPVersion {
    char *str;
    GList/*<char *>*/ *minor;
    GList/*<VersionSuffix>*/ *suffixes;
    char *revision;
    char *major;
    /*@refs@*/ unsigned int refs;
    char letter;
};

static void
normalize_minor(char *str) {
    int i;
    if (str[0] != '0') {
        return;
    }

    i = strlen(str) - 1;
    while (i >= 0) {
        if (str[i] != '0') {
            break;
        }
        str[i] = '\0';
        --i;
    }
}

static CPVersion
cp_version_alloc(
    char *major,
    /*@null@*/ GList *minor,
    char letter,
    /*@null@*/ GList *suffixes,
    char *revision
) {
    CPVersion result;
    GString *str;

    result = g_new(struct CPVersion, 1);
    result->refs = 1;

    str = g_string_new(major);
    CP_GLIST_ITER(minor, elem) {
        g_string_append_c(str, '.');
        g_string_append(str, elem);
        /* Attention */
        normalize_minor(elem);
    } end_CP_GLIST_ITER
    if (letter != '\0') {
        g_string_append_c(str, letter);
    }
    CP_GLIST_ITER(suffixes, elem) {
        VersionSuffix suffix = elem;
        switch (suffix->type) {
            case SUF_ALPHA:
                g_string_append(str, "_alpha");
                break;
            case SUF_BETA:
                g_string_append(str, "_beta");
                break;
            case SUF_PRE:
                g_string_append(str, "_pre");
                break;
            case SUF_RC:
                g_string_append(str, "_rc");
                break;
            case SUF_P:
                g_string_append(str, "_p");
                break;
            default:
                g_assert_not_reached();
        }
        if (suffix->value != NULL) {
            g_string_append(str, suffix->value);
        }
    } end_CP_GLIST_ITER
    if (revision != NULL) {
        g_string_append(str, "-r");
        g_string_append(str, revision);
    }
    result->str = g_string_free(str, FALSE);

    result->major = major;
    result->minor = minor;
    result->letter = letter;
    result->suffixes = suffixes;
    result->revision = revision == NULL ? g_strdup("0") : revision;

    return result;
}

static VersionSuffix
suffix_alloc(VersionSuffixType type, char *value) {
    VersionSuffix result = g_new(struct VersionSuffix, 1);

    result->type = type;
    result->value = value;

    return result;
}

%}

%token <str> UPPER NUMBER
%token <chr> LOWER
%token ATOM_MAGIC PV_MAGIC CATEGORY_MAGIC VERSION_MAGIC SLOT_MAGIC REPO_MAGIC
%token PLUS MINUS UNDERLINE LT GT EQ TILDE STAR DOT COLON SLASH LSQUARE RSQUARE
%token ALPHA BETA RC PRE P R COMMA AT EXCL QMARK

%type <atom> atom base_atom cp cpv atom_slot_repo
%type <version> version
%type <pv> pv
%type <str_list> version_minor_loop
%type <chr> maybe_letter
%type <suffix_list> suffix_loop
%type <suffix_type> suffix_type

%type <str> category slot repo package package_
%type <str> maybe_revision maybe_number use_name
%type <str> word word_or_plus word_no_underline
%type <str> word_or_minus word_or_minus_loop
%type <str> word_or_plus_minus word_or_plus_minus_loop
%type <str> word_or_plus_minus_dot word_or_plus_minus_dot_loop
%type <str> word_or_plus_minus_at word_or_plus_minus_at_loop

%%

start:
    ATOM_MAGIC atom { ctx->atom = $2; }
  | PV_MAGIC pv { ctx->pv.package = $2.package; ctx->pv.version = $2.version; }
  | CATEGORY_MAGIC category { g_free($2); }
  | VERSION_MAGIC version { ctx->version = $2; }
  | SLOT_MAGIC slot { g_free($2); }
  | REPO_MAGIC repo { g_free($2); }

atom:
    atom_slot_repo
  | atom_slot_repo LSQUARE use_loop RSQUARE { $$ = $1; }

atom_slot_repo:
    base_atom { $$ = $1; $$->slot = NULL;    $$->repo = NULL; }
  | base_atom COLON slot { $$ = $1; $$->slot = $3; $$->repo = NULL; }
  | base_atom COLON COLON repo { $$ = $1; $$->slot = NULL; $$->repo = $4; }
  | base_atom COLON slot COLON COLON repo
      { $$ = $1; $$->slot = $3; $$->repo = $6; }

base_atom:
    cp             { $$ = $1; $$->op = OP_NONE; }
  | LT    cpv      { $$ = $2; $$->op = OP_LT; }
  | LT EQ cpv      { $$ = $3; $$->op = OP_LE; }
  |    EQ cpv      { $$ = $2; $$->op = OP_EQ; }
  | GT EQ cpv      { $$ = $3; $$->op = OP_GE; }
  | GT    cpv      { $$ = $2; $$->op = OP_GT; }
  | EQ    cpv STAR { $$ = $2; $$->op = OP_GLOB; }
  | TILDE cpv      { $$ = $2; $$->op = OP_TILDE; }

cp:
    category SLASH package { $$ = cp_atom_alloc($1, $3, NULL); }

cpv:
    category SLASH pv { $$ = cp_atom_alloc($1, $3.package, $3.version); }

pv:
    package MINUS version { $$.package = $1; $$.version = $3; }

category:
    word_or_plus
  | word_or_plus word_or_plus_minus_dot_loop { $$ = DOCONCAT2($1, $2); }

package:
    package_ %dprec 1
  | package_ MINUS version
      { $$ = NULL; g_free($1);cp_version_unref($3); YYABORT; } %dprec 2

package_:
    word_or_plus
  | word_or_plus word_or_plus_minus_loop { $$ = DOCONCAT2($1, $2); }

version:
    NUMBER version_minor_loop maybe_letter suffix_loop maybe_revision
      { $$ = cp_version_alloc($1, $2, $3, $4, $5); }

maybe_letter:
    /* empty */ { $$ = '\0'; }
  | P           { $$ = 'p'; }
  | R           { $$ = 'r'; }
  | LOWER       { $$ = $1; }

maybe_number:
    /* empty */    { $$ = NULL; }
  | NUMBER

maybe_revision:
    /* empty */    { $$ = NULL; }
  | MINUS R NUMBER { $$ = $3; }

version_minor_loop:
    /* empty */ { $$ = NULL; }
  | version_minor_loop DOT NUMBER { $$ = g_list_append($1, $3); }

suffix_loop:
    /* empty */ { $$ = NULL; }
  | suffix_loop UNDERLINE suffix_type maybe_number
      { $$ = g_list_append($1, suffix_alloc($3, $4)); }

suffix_type:
    ALPHA { $$ = SUF_ALPHA; }
  | BETA  { $$ = SUF_BETA;  }
  | PRE   { $$ = SUF_PRE;   }
  | RC    { $$ = SUF_RC;    }
  | P     { $$ = SUF_P;     }

slot:
    word_or_plus
  | word_or_plus word_or_plus_minus_dot_loop { $$ = DOCONCAT2($1, $2); }

repo:
    word
  | word word_or_minus_loop { $$ = DOCONCAT2($1, $2); }

use_loop:
    use_item
  | use_loop COMMA use_item

/* TODO: save useflags */
use_item:
    use_name { g_free($1); }
  | MINUS use_name { g_free($2); }
  | use_name EQ { g_free($1); }
  | use_name QMARK { g_free($1); }
  | EXCL use_name EQ { g_free($2); }
  | EXCL use_name QMARK { g_free($2); }

use_name:
    word_no_underline
  | word_no_underline word_or_plus_minus_at_loop { $$ = DOCONCAT2($1, $2); }

/* String parts with no special meaning follow */

word_no_underline:
    UPPER
  | LOWER { $$ = g_strnfill(1, $1); }
  | NUMBER
  | ALPHA { $$ = g_strdup("alpha"); }
  | BETA { $$ = g_strdup("beta"); }
  | PRE { $$ = g_strdup("pre"); }
  | RC { $$ = g_strdup("rc"); }
  | P { $$ = g_strnfill(1, 'p'); }
  | R { $$ = g_strnfill(1, 'r'); }

word:
    word_no_underline
  | UNDERLINE { $$ = g_strnfill(1, '_'); }

word_or_plus:
    word
  | PLUS { $$ = g_strnfill(1, '+'); }

word_or_minus:
    word
  | MINUS { $$ = g_strnfill(1, '-'); }
word_or_minus_loop:
    word_or_minus
  | word_or_minus_loop word_or_minus { $$ = DOCONCAT2($1, $2); }

word_or_plus_minus:
    word_or_plus
  | MINUS { $$ = g_strnfill(1, '-'); }
word_or_plus_minus_loop:
    word_or_plus_minus
  | word_or_plus_minus_loop word_or_plus_minus { $$ = DOCONCAT2($1, $2); }

word_or_plus_minus_at:
    word_or_plus_minus
  | AT { $$ = g_strnfill(1, '@'); }
word_or_plus_minus_at_loop:
    word_or_plus_minus_at
  | word_or_plus_minus_at_loop word_or_plus_minus_at { $$ = DOCONCAT2($1, $2); }

word_or_plus_minus_dot:
    word_or_plus_minus
  | DOT { $$ = g_strnfill(1, '.'); }
word_or_plus_minus_dot_loop:
    word_or_plus_minus_dot
  | word_or_plus_minus_dot_loop word_or_plus_minus_dot
      { $$ = DOCONCAT2($1, $2); }

%%

static gboolean G_GNUC_WARN_UNUSED_RESULT
doparse(cp_atomparser_ctx *ctx, const char *value, int magic) {
    YY_BUFFER_STATE bp;
    gboolean result;

    ctx->magic = magic;

    cp_atomparser_lex_init(&ctx->yyscanner);
    bp = cp_atomparser__scan_string(value, ctx->yyscanner);
    cp_atomparser__switch_to_buffer(bp, ctx->yyscanner);
    cp_atomparser_set_extra(ctx, ctx->yyscanner);

    if (cp_string_is_true(g_getenv("CPORTAGE_ATOMPARSER_DEBUG"))) {
        cp_atomparser_debug = 1;
        cp_atomparser_set_debug(1, ctx->yyscanner);
    }

    result = cp_atomparser_parse(ctx) == 0;
    cp_atomparser__delete_buffer(bp, ctx->yyscanner);
    cp_atomparser_lex_destroy(ctx->yyscanner);

    return result;
}

CPVersion
cp_version_new(const char *value, GError **error) {
    cp_atomparser_ctx ctx;

    g_assert(error == NULL || *error == NULL);

    ctx.version = NULL;

    if (!doparse(&ctx, value, VERSION_MAGIC)) {
        cp_version_unref(ctx.version);
        g_set_error(error, CP_ERROR, (gint)CP_ERROR_ATOM_SYNTAX,
            _("'%s': invalid version"), value);
        return FALSE;
    }

    g_assert(ctx.version != NULL);

    return ctx.version;
}

CPVersion
cp_version_ref(CPVersion self) {
    ++self->refs;
    /*@-refcounttrans@*/
    return self;
    /*@=refcounttrans@*/
}

void
cp_version_unref(CPVersion self) {
    if (self == NULL) {
        /*@-mustfreeonly@*/
        return;
        /*@=mustfreeonly@*/
    }
    g_assert(self->refs > 0);
    if (--self->refs == 0) {
        g_free(self->str);
        g_free(self->major);
        g_list_free_full(self->minor, g_free);
        g_list_free_full(self->suffixes, (GDestroyNotify)suffix_free);
        g_free(self->revision);

        /*@-refcounttrans@*/
        g_free(self);
        /*@=refcounttrans@*/
    }
}

const char *
cp_version_str(const CPVersion self) {
    return self->str;
}

static inline int
int_cmp(int x, int y) {
    return (x > y) - (x < y);
}

static inline int
num_cmp(const char *first, const char *second) {
    mpz_t f;
    mpz_t s;
    int result;

    /* TODO: precalculate values during CPVersion creation? */
    mpz_init_set_str(f, first, 10);
    mpz_init_set_str(s, second, 10);

    result = mpz_cmp(f, s);

    mpz_clear(f);
    mpz_clear(s);

    return result;
}

static int
cp_version_cmp_internal(
    CPVersion first,
    CPVersion second,
    gboolean check_revision
) {
    GList *first_iter;
    GList *second_iter;
    int result;

    /* Compare major version */
    result = num_cmp(first->major, second->major);
    if (result != 0) {
        return result;
    }

    /* Compare minor version components */
    first_iter = first->minor;
    second_iter = second->minor;

    while (first_iter != NULL && second_iter != NULL) {
        char *f = first_iter->data;
        char *s = second_iter->data;

        if (f[0] == '0' || s[0] == '0') {
            /*
              We already stripped off trailing zeroes when created a CPVersion
             */
            result = g_strcmp0(f, s);
        } else {
            result = num_cmp(f, s);
        }
        if (result != 0) {
          return result;
        }

        first_iter = first_iter->next;
        second_iter = second_iter->next;
    }

    result = int_cmp(first_iter != NULL, second_iter != NULL);
    if (result != 0) {
        return result;
    }

    /* Compare letter */
    result = int_cmp(first->letter, second->letter);
    if (result != 0) {
        return result;
    }

    /* Compare suffixes */
    first_iter = first->suffixes;
    second_iter = second->suffixes;

    while (first_iter != NULL || second_iter != NULL) {
        VersionSuffix f = first_iter == NULL ? NULL : first_iter->data;
        VersionSuffix s = second_iter == NULL ? NULL : second_iter->data;

        if (f == NULL) {
            return s->type == SUF_P ? -1 : 1;
        } else if (s == NULL) {
            return f->type == SUF_P ? 1 : -1;
        }

        result = int_cmp(f->type, s->type);
        if (result != 0) {
            return result;
        }

        result = num_cmp(
            f->value == NULL ? "0" : f->value,
            s->value == NULL ? "0" : s->value);
        if (result != 0) {
            return result;
        }

        if (first_iter != NULL) {
            first_iter = first_iter->next;
        }
        if (second_iter != NULL) {
            second_iter = second_iter->next;
        }
    }

    /* Compare revision */
    if (!check_revision) {
        return 0;
    }

    return num_cmp(first->revision, second->revision);
}

int
cp_version_cmp(CPVersion first, CPVersion second) {
    return cp_version_cmp_internal(first, second, TRUE);
}

static gboolean
cp_version_tilde_match(const CPVersion first, const CPVersion second) {
    if (cp_version_cmp_internal(first, second, FALSE) != 0) {
        return FALSE;
    }

    /* Portage violates PMS here, it just ignores revision */
    return num_cmp(first->revision, second->revision) <= 0;
}

static gboolean
cp_version_glob_match(const CPVersion first, const CPVersion second) {
    /* Portage does some hackery with leading zeroes here. Why? */
    return g_str_has_prefix(second->str, first->str);
}

CPAtom
cp_atom_new(const char *value, GError **error) {
    cp_atomparser_ctx ctx;

    g_assert(error == NULL || *error == NULL);

    ctx.atom = NULL;

    if (!doparse(&ctx, value, ATOM_MAGIC)) {
        cp_atom_unref(ctx.atom);
        g_set_error(error, CP_ERROR, (gint)CP_ERROR_ATOM_SYNTAX,
            _("'%s': invalid atom"), value);
        return FALSE;
    }

    g_assert(ctx.atom != NULL);

    return ctx.atom;
}

gboolean
cp_atom_category_validate(const char *value, GError **error) {
    cp_atomparser_ctx ctx;

    g_assert(error == NULL || *error == NULL);

    if (!doparse(&ctx, value, CATEGORY_MAGIC)) {
        g_set_error(error, CP_ERROR, (gint)CP_ERROR_ATOM_SYNTAX,
            _("'%s': invalid category name"), value);
        return FALSE;
    }

    return TRUE;
}

gboolean
cp_atom_slot_validate(const char *value, GError **error) {
    cp_atomparser_ctx ctx;

    g_assert(error == NULL || *error == NULL);

    if (!doparse(&ctx, value, SLOT_MAGIC)) {
        g_set_error(error, CP_ERROR, (gint)CP_ERROR_ATOM_SYNTAX,
            _("'%s': invalid slot name"), value);
        return FALSE;
    }

    return TRUE;
}

gboolean
cp_atom_repo_validate(const char *value, GError **error) {
    cp_atomparser_ctx ctx;

    g_assert(error == NULL || *error == NULL);

    if (!doparse(&ctx, value, REPO_MAGIC)) {
        g_set_error(error, CP_ERROR, (gint)CP_ERROR_ATOM_SYNTAX,
            _("'%s': invalid repository name"), value);
        return FALSE;
    }

    return TRUE;
}

gboolean
cp_atom_pv_split(
    const char *value,
    char **name,
    CPVersion *version,
    GError **error
) {
    cp_atomparser_ctx ctx;

    g_assert(error == NULL || *error == NULL);

    ctx.pv.package = NULL;
    ctx.pv.version = NULL;

    if (!doparse(&ctx, value, PV_MAGIC)) {
        g_set_error(error, CP_ERROR, (gint)CP_ERROR_ATOM_SYNTAX,
            _("'%s' isn't valid package-version"), value);
        return FALSE;
    }

    g_assert(ctx.pv.package != NULL);
    g_assert(ctx.pv.version != NULL);

    *name = ctx.pv.package;
    *version = ctx.pv.version;

    return TRUE;
}

CPAtom
cp_atom_ref(CPAtom self) {
    ++self->refs;
    /*@-refcounttrans@*/
    return self;
    /*@=refcounttrans@*/
}

void
cp_atom_unref(CPAtom self) {
    if (self == NULL) {
        /*@-mustfreeonly@*/
        return;
        /*@=mustfreeonly@*/
    }
    g_assert(self->refs > 0);
    if (--self->refs == 0) {
        g_free(self->category);
        g_free(self->package);
        cp_version_unref(self->version);
        g_free(self->slot);
        g_free(self->repo);

        /*@-refcounttrans@*/
        g_free(self);
        /*@=refcounttrans@*/
    }
}

const char *
cp_atom_category(const CPAtom self) {
    return self->category;
}

const char *
cp_atom_package(const CPAtom self) {
    return self->package;
}

gboolean
cp_atom_matches(const CPAtom self, const CPPackage package) {
    gboolean result;
    CPVersion pkg_version;

    if (g_strcmp0(self->category, cp_package_category(package)) != 0) {
        return FALSE;
    }
    if (g_strcmp0(self->package, cp_package_name(package)) != 0) {
        return FALSE;
    }
    if (self->slot != NULL
            && g_strcmp0(self->slot, cp_package_slot(package)) != 0) {
        return FALSE;
    }
    /*
      TODO: in portage, package without repository can be matched by atom
      with any repository. Does anything specify this behaviour?
     */
    if (self->repo != NULL
            && g_strcmp0(self->repo, cp_package_repo(package)) != 0) {
        return FALSE;
    }

    pkg_version = cp_package_version(package);
    switch (self->op) {
        case OP_NONE:
            result = TRUE;
            break;
        case OP_LT:
            result = cp_version_cmp(self->version, pkg_version) < 0;
            break;
        case OP_LE:
            result = cp_version_cmp(self->version, pkg_version) <= 0;
            break;
        case OP_EQ:
            result = cp_version_cmp(self->version, pkg_version) == 0;
            break;
        case OP_GE:
            result = cp_version_cmp(self->version, pkg_version) >= 0;
            break;
        case OP_GT:
            result = cp_version_cmp(self->version, pkg_version) > 0;
            break;
        case OP_TILDE:
            result = cp_version_tilde_match(self->version, pkg_version);
            break;
        case OP_GLOB:
            result = cp_version_glob_match(self->version, pkg_version);
            break;
        default:
            g_assert_not_reached();
    }
    cp_version_unref(pkg_version);

    /* TODO: check useflags */

    return result;
}
