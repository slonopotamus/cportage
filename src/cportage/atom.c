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

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "cportage/atom.h"

typedef enum OP_TYPE {
    OP_NONE,
    OP_LT,
    OP_LE,
    OP_EQ,
    OP_GE,
    OP_GT,
    OP_TILDE,
    OP_STAR
} OpType;

struct CPortageAtom {
    /*@refs@*/ int refs;
    OpType operator;
    /*@only@*/ char *category;
    /*@only@*/ char *package;
    /* Nullable */
    /*@only@*/ char *slot;
};

/*@only@*/ static struct {
  /*@only@*/ GRegex *regex;
  int op_idx, star_idx, simple_idx, slot_idx, use_idx;
} *atom_re;

static void
init_atom_re(void) /*@globals undef atom_re@*/ /*@modifies atom_re@*/ {
    /*
        2.1.1 A category name may contain any of the characters [A-Za-z0-9+_.-].
        It must not begin with a hyphen or a dot.
     */
    const char *cat = "([\\w+][\\w+.-]*)";
    /*
        2.1.2 A package name may contain any of the characters [A-Za-z0-9+_-].
        It must not begin with a hyphen,
        and must not end in a hyphen followed by _valid version string_.
    */
    const char *pkg = "([\\w+][\\w+-]*?)";
    /*
        2.1.3 A slot name may contain any of the characters [A-Za-z0-9+_.-].
        It must not begin with a hyphen or a dot.
     */
    const char *slot = "(?P<slot>[\\w+][\\w+.-]*)";
    /*
        2.1.4 A USE flag name may contain any of the characters [A-Za-z0-9+_@-].
        It must begin with an alphanumeric character.
     */
    const char *use_name = "[A-Za-z0-9][\\w+@-]*";
    /* See 2.2 section for version syntax. */
    const char *ver = "\\d+(\\.\\d+)*[a-z]?(_(pre|p|beta|alpha|rc)\\d*)*(-r\\d+)?";
    /* TODO: add reference to PMS */
    const char *op = "(?P<op>[=~]|[><]=?)";
    char *use_item = g_strdup_printf("(?:!?%s[=?]|-?%s)", use_name, use_name);
    char *use = g_strdup_printf("(?P<use>\\[%s(?:,%s)*\\])?", use_item, use_item);
    char *cp = g_strdup_printf("(%s/%s(-%s)\?\?)", cat, pkg, ver);
    char *cpv = g_strdup_printf("%s-%s", cp, ver);
    char *atom_re_str = g_strdup_printf("^(?:(?:%s%s)|(?P<star>=%s\\*)|(?P<simple>%s))(?::%s)?%s$",
                 op, cpv, cpv, cp, slot, use);
    /*@null@*/ GError *error = NULL;
    free(use_item);
    free(use);
    free(cp);
    free(cpv);

    atom_re = g_malloc(sizeof(*atom_re));

    atom_re->regex = g_regex_new(atom_re_str, 0, 0, &error);
    g_assert_no_error(error);

    free(atom_re_str);

    atom_re->op_idx = g_regex_get_string_number(atom_re->regex, "op");
    atom_re->star_idx = g_regex_get_string_number(atom_re->regex, "star");
    atom_re->simple_idx = g_regex_get_string_number(atom_re->regex, "simple");
    atom_re->slot_idx = g_regex_get_string_number(atom_re->regex, "slot");
    atom_re->use_idx = g_regex_get_string_number(atom_re->regex, "use");
    g_assert(atom_re->op_idx != -1
        && atom_re->star_idx != -1
        && atom_re->simple_idx != -1
        && atom_re->slot_idx != -1
        && atom_re->use_idx != -1);
}

CPortageAtom
cportage_atom_new(const char *str, GError **error) {
    CPortageAtom atom;
    GMatchInfo *match;
    char *invalid_version;

    g_assert(error == NULL || *error == NULL);

    if (atom_re == NULL)
        init_atom_re();

    if (g_regex_match_full(atom_re->regex, str, strlen(str), 0, 0, &match, error)) {
        char *type;
        OpType op;
        int cat_idx;
        if ((type = g_match_info_fetch(match, atom_re->op_idx)) != NULL) {
            cat_idx = atom_re->op_idx + 2;
            if (strcmp(type, "<") == 0) {
                op = OP_LT;
            } else if (strcmp(type, "<=") == 0) {
                op = OP_LE;
            } else if (strcmp(type, "=") == 0) {
                op = OP_EQ;
            } else if (strcmp(type, ">=") == 0) {
                op = OP_GE;
            } else if (strcmp(type, ">") == 0) {
                op = OP_GT;
            } else if (strcmp(type, "~") == 0) {
                op = OP_TILDE;
            } else {
                g_assert_not_reached();
            }
        } else if ((type = g_match_info_fetch(match, atom_re->star_idx)) != NULL) {
            cat_idx = atom_re->star_idx + 2;
            op = OP_STAR;
        } else if ((type = g_match_info_fetch(match, atom_re->simple_idx)) != NULL) {
            cat_idx = atom_re->simple_idx + 2;
            op = OP_NONE;
        } else {
            /* Getting here means we have a bug in atom regex */
            g_assert_not_reached();
            cat_idx = -1;
        }
        free(type);

        if ((invalid_version = g_match_info_fetch(match, cat_idx + 2)) != NULL) {
            /* Pkg name ends with version string, that's disallowed */
            free(invalid_version);
            return NULL;
        }
        atom = g_malloc(sizeof(*atom));
        atom->refs = 1;
        atom->operator = op;

        atom->category = g_match_info_fetch(match, cat_idx);
        g_assert(atom->category != NULL);

        atom->package = g_match_info_fetch(match, cat_idx + 1);
        g_assert(atom->package != NULL);

        atom->slot = g_match_info_fetch(match, atom_re->slot_idx);
        g_assert(atom->slot != NULL);
        /* TODO: store version and useflags */
    } else
        atom = NULL;
    g_match_info_free(match);
    return atom;
}

CPortageAtom
cportage_atom_ref(CPortageAtom self) {
    ++self->refs;
    return self;
}

void
cportage_atom_unref(CPortageAtom self) {
    g_return_if_fail(self != NULL);
    g_assert(self->refs > 0);
    if (--self->refs == 0) {
        free(self->category);
        free(self->package);
        free(self->slot);
        /*@-refcounttrans@*/
        free(self);
        /*@=refcounttrans@*/
    }
}
