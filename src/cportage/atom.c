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

#include <cportage/atom.h>

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

struct CPAtom {
    /*@refs@*/ int refs;
    OpType op;
    /*@only@*/ char *category;
    /*@only@*/ char *package;
    /* Nullable */
    /*@only@*/ char *slot;
};

typedef struct AtomRegex {
  /*@only@*/ GRegex *regex;
  int op_idx, star_idx, simple_idx, slot_idx, use_idx;
} *AtomRegex;

/*
    2.1.1 A category name may contain any of the characters [A-Za-z0-9+_.-].
    It must not begin with a hyphen or a dot.
 */
#define CAT "([\\w+][\\w+.-]*)"
/*
    2.1.2 A package name may contain any of the characters [A-Za-z0-9+_-].
    It must not begin with a hyphen,
    and must not end in a hyphen followed by _valid version string_.
 */
#define PKG "([\\w+][\\w+-]*?)"
/*
    2.1.3 A slot name may contain any of the characters [A-Za-z0-9+_.-].
    It must not begin with a hyphen or a dot.
 */
#define SLOT "(?P<slot>[\\w+][\\w+.-]*)"
/*
    2.1.4 A USE flag name may contain any of the characters [A-Za-z0-9+_@-].
    It must begin with an alphanumeric character.
 */
#define USE_NAME "[A-Za-z0-9][\\w+@-]*"
/* See 2.2 section for version syntax. */
#define VER "\\d+(\\.\\d+)*[a-z]?(_(pre|p|beta|alpha|rc)\\d*)*(-r\\d+)?"
/* TODO: add reference to PMS */
#define OP "(?P<op>[=~]|[><]=?)"
#define USE_ITEM "(?:!?" USE_NAME "[=?]|-?" USE_NAME ")"
#define USE "(?P<use>\\[" USE_ITEM "(?:," USE_ITEM ")*\\])?"
#define CP "(" CAT "/" PKG "(-" VER ")\?\?)"
#define CPV CP "-" VER
#define ATOM "^(?:(?:" OP CPV ")|(?P<star>=" CPV "\\*)|(?P<simple>" CP "))(?::" SLOT")?" USE "$"

static char * G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT
safe_fetch(const GMatchInfo *match_info, int match_num) /*@*/ {
    char *result = g_match_info_fetch(match_info, match_num);

    /*
        Workaround for gregex bug:
        https://bugzilla.gnome.org/show_bug.cgi?id=588217
     */
    if (result == NULL && match_num >= 0) {
        const GRegex *regex = g_match_info_get_regex(match_info);
        const int capture_count = g_regex_get_capture_count(regex);
        g_assert(match_num <= capture_count);
        /*@-mustfreefresh@*/
        result = g_strdup("");
        /*@=mustfreefresh@*/
    }

    g_assert(result != NULL);
    return result;
}

CPAtom
cp_atom_new(const char *str, GError **error) {
    static struct {
        /*@only@*/ GRegex *regex;
        int op_idx, star_idx, simple_idx, slot_idx, use_idx;
    } atom_re;

    CPAtom atom;
    GMatchInfo *match;
    char *invalid_version;

    g_assert(error == NULL || *error == NULL);
    g_assert(g_utf8_validate(str, -1, NULL));

    if (atom_re.regex == NULL) {
        /*@-mustfreeonly@*/
        atom_re.regex = g_regex_new(ATOM, (int)G_REGEX_OPTIMIZE, 0, error);
        /*@=mustfreeonly@*/
        if (atom_re.regex == NULL) {
            return NULL;
        }

        atom_re.op_idx = g_regex_get_string_number(atom_re.regex, "op");
        atom_re.star_idx = g_regex_get_string_number(atom_re.regex, "star");
        atom_re.simple_idx = g_regex_get_string_number(atom_re.regex, "simple");
        atom_re.slot_idx = g_regex_get_string_number(atom_re.regex, "slot");
        atom_re.use_idx = g_regex_get_string_number(atom_re.regex, "use");

        g_assert(atom_re.op_idx != -1);
        g_assert(atom_re.star_idx != -1);
        g_assert(atom_re.simple_idx != -1);
        g_assert(atom_re.slot_idx != -1);
        g_assert(atom_re.use_idx != -1);
    }

    if (g_regex_match_full(atom_re.regex, str, -1, 0, 0, &match, error)) {
        char *op_match = NULL;
        char *star_match = NULL;
        char *simple_match = NULL;
        OpType op;
        int cat_idx;

        if ((op_match = safe_fetch(match, atom_re.op_idx))[0] != '\0') {
            cat_idx = atom_re.op_idx + 2;
            if (g_utf8_collate(op_match, "<") == 0) {
                op = OP_LT;
            } else if (g_utf8_collate(op_match, "<=") == 0) {
                op = OP_LE;
            } else if (g_utf8_collate(op_match, "=") == 0) {
                op = OP_EQ;
            } else if (g_utf8_collate(op_match, ">=") == 0) {
                op = OP_GE;
            } else if (g_utf8_collate(op_match, ">") == 0) {
                op = OP_GT;
            } else if (g_utf8_collate(op_match, "~") == 0) {
                op = OP_TILDE;
            } else {
                /*@-type@*/
                op = -1;
                /*@=type@*/
                g_assert_not_reached();
            }
        } else if ((star_match = safe_fetch(match, atom_re.star_idx))[0] != '\0') {
            cat_idx = atom_re.star_idx + 2;
            op = OP_STAR;
        } else if ((simple_match = safe_fetch(match, atom_re.simple_idx))[0] != '\0') {
            cat_idx = atom_re.simple_idx + 2;
            op = OP_NONE;
        } else {
            /* Getting here means we have a bug in atom regex */
            /*@-type@*/
            op = cat_idx = -1;
            /*@=type@*/
            g_assert_not_reached();
        }
        g_free(op_match);
        g_free(star_match);
        g_free(simple_match);

        if ((invalid_version = safe_fetch(match, cat_idx + 2))[0] != '\0') {
            /* Pkg name ends with version string, that's disallowed */
            g_free(invalid_version);
            return NULL;
        }
        g_free(invalid_version);

        atom = g_new(struct CPAtom, 1);
        atom->refs = 1;
        atom->op = op;
        /*@-mustfreeonly@*/
        atom->category = safe_fetch(match, cat_idx);
        atom->package = safe_fetch(match, cat_idx + 1);
        atom->slot = safe_fetch(match, atom_re.slot_idx);
        /*@=mustfreeonly@*/

        /* TODO: store version and useflags */
    } else {
        atom = NULL;
    }
    g_match_info_free(match);
    return atom;
}

CPAtom
cp_atom_ref(CPAtom self) {
    ++self->refs;
    return self;
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
        g_free(self->slot);
        /*@-refcounttrans@*/
        g_free(self);
        /*@=refcounttrans@*/
    }
}
