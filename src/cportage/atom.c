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

#include <cportage/atom.h>
#include <cportage/error.h>
#include <cportage/macros.h>
#include <cportage/version.h>

typedef enum OP_TYPE {
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
    /*@only@*/ char *slot;
    /*@only@*/ char *repo;

    /*@refs@*/ int refs;
    OpType op;
};

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
#define PKG_FULL PKG "(-" VER ")\?\?"
#define CP "(" CAT "/" PKG_FULL ")"
#define PV PKG_FULL "-(" VER ")"
#define REPO "(?P<repo>[\\w][\\w-]*)"

#define CPV CP "-" VER
#define ATOM "^(?:(?:" OP CPV ")|(?P<glob>=" CPV "\\*)|(?P<simple>" CP "))" \
    "(?::" SLOT ")?" \
    "(?:::" REPO ")?" \
    USE "$"

/*@-checkpost@*/
static /*@nullterminated@*/ /*@only@*/ char *
G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT
safe_fetch(
    const GMatchInfo *match_info,
    int match_num
) /*@*/ /*@ensures maxRead(result) >= 1@*/ {
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
/*@=checkpost@*/

static gboolean G_GNUC_WARN_UNUSED_RESULT
check_invalid_version(
    const GMatchInfo *match,
    int index,
    /*@null@*/ GError **error
) /*@modifies *error@*/ {
    char *invalid_version;
    gboolean result;

    g_assert(error == NULL || *error == NULL);

    invalid_version = safe_fetch(match, index);
    result = invalid_version[0] == '\0';
    if (!result) {
        g_set_error(error, CP_ERROR, (gint)CP_ERROR_ATOM_SYNTAX,
            _("'%s' contains version in package name"),
            g_match_info_get_string(match));
    }
    g_free(invalid_version);

    return result;
}

/* TODO: add caching */
CPAtom
cp_atom_new(const char *value, GError **error) {
    static struct {
        /*@only@*/ GRegex *regex;
        int op_idx, glob_idx, simple_idx, slot_idx, repo_idx, use_idx;
    } atom_re;

    CPAtom self = NULL;
    GMatchInfo *match;
    GError *tmp_error = NULL;

    char *op_match = NULL;
    char *glob_match = NULL;
    char *simple_match = NULL;
    OpType op;
    int cat_idx;

    g_assert(error == NULL || *error == NULL);

    if (atom_re.regex == NULL) {
        /*@-mustfreeonly@*/
        atom_re.regex = g_regex_new(ATOM, (int)G_REGEX_OPTIMIZE, 0, error);
        /*@=mustfreeonly@*/
        if (atom_re.regex == NULL) {
            g_assert_not_reached();
        }

        atom_re.op_idx = g_regex_get_string_number(atom_re.regex, "op");
        atom_re.glob_idx = g_regex_get_string_number(atom_re.regex, "glob");
        atom_re.simple_idx = g_regex_get_string_number(atom_re.regex, "simple");
        atom_re.slot_idx = g_regex_get_string_number(atom_re.regex, "slot");
        atom_re.repo_idx = g_regex_get_string_number(atom_re.regex, "repo");
        atom_re.use_idx = g_regex_get_string_number(atom_re.regex, "use");

        g_assert(atom_re.op_idx > 0);
        g_assert(atom_re.glob_idx > 0);
        g_assert(atom_re.simple_idx > 0);
        g_assert(atom_re.slot_idx > 0);
        g_assert(atom_re.repo_idx > 0);
        g_assert(atom_re.use_idx > 0);
    }

    if (!g_regex_match_full(atom_re.regex, value, (gssize)-1, 0, 0, &match, &tmp_error)) {
        g_assert_no_error(tmp_error);
        g_set_error(error, CP_ERROR, (gint)CP_ERROR_ATOM_SYNTAX,
            _("'%s': invalid atom"), value);
        goto OUT;
    }

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
            g_assert_not_reached();
        }
    } else if ((glob_match = safe_fetch(match, atom_re.glob_idx))[0] != '\0') {
        cat_idx = atom_re.glob_idx + 2;
        op = OP_GLOB;
    } else if ((simple_match = safe_fetch(match, atom_re.simple_idx))[0] != '\0') {
        cat_idx = atom_re.simple_idx + 2;
        op = OP_NONE;
    } else {
        /* Getting here means we have a bug in atom regex */
        g_assert_not_reached();
    }
    g_free(op_match);
    g_free(glob_match);
    g_free(simple_match);

    if (!check_invalid_version(match, cat_idx + 2, error)) {
        goto OUT;
    }

    self = g_new0(struct CPAtom, 1);
    self->refs = 1;
    self->op = op;
    g_assert(self->category == NULL);
    self->category = safe_fetch(match, cat_idx);
    g_assert(self->package == NULL);
    self->package = safe_fetch(match, cat_idx + 1);
    g_assert(self->slot == NULL);
    self->slot = safe_fetch(match, atom_re.slot_idx);
    g_assert(self->repo == NULL);
    self->repo = safe_fetch(match, atom_re.repo_idx);

    /* TODO: store version and useflags */

OUT:
    g_match_info_free(match);
    return self;
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
    if (self->slot[0] != '\0' && g_strcmp0(self->slot, cp_package_slot(package)) != 0) {
        return FALSE;
    }
    if (self->repo[0] != '\0' && g_strcmp0(self->repo, cp_package_repo(package)) != 0) {
        return FALSE;
    }

    pkg_version = cp_package_version(package);
    switch (self->op) {
        case OP_NONE:
            result = TRUE;
            break;
        case OP_LT:
            g_assert(self->version != NULL);
            result = cp_version_cmp(self->version, pkg_version) < 0;
            break;
        case OP_LE:
            g_assert(self->version != NULL);
            result = cp_version_cmp(self->version, pkg_version) <= 0;
            break;
        case OP_EQ:
            g_assert(self->version != NULL);
            result = cp_version_cmp(self->version, pkg_version) == 0;
            break;
        case OP_GE:
            g_assert(self->version != NULL);
            result = cp_version_cmp(self->version, pkg_version) >= 0;
            break;
        case OP_GT:
            g_assert(self->version != NULL);
            result = cp_version_cmp(self->version, pkg_version) > 0;
            break;
        case OP_TILDE:
            g_assert(self->version != NULL);
            result = cp_version_any_revision_match(self->version, pkg_version);
            break;
        case OP_GLOB:
            g_assert(self->version != NULL);
            result = cp_version_glob_match(self->version, pkg_version);
            break;
        default:
            g_assert_not_reached();
    }
    cp_version_unref(pkg_version);

    /* TODO: check useflags */

    return result;
}

gboolean
cp_atom_category_validate(const char *category, GError **error) {
    /*@only@*/ static GRegex *regex = NULL;

    GError *tmp_error = NULL;

    g_assert(error == NULL || *error == NULL);

    if (regex == NULL) {
        /*@-mustfreeonly@*/
        regex = g_regex_new("^" CAT "$", (int)G_REGEX_OPTIMIZE, 0, error);
        /*@=mustfreeonly@*/
        if (regex == NULL) {
            g_assert_not_reached();
        }
    }

    if (!g_regex_match_full(regex, category, (gssize)-1, 0, 0, NULL, &tmp_error)) {
        g_assert_no_error(tmp_error);
        g_set_error(error, CP_ERROR, (gint)CP_ERROR_ATOM_SYNTAX,
            _("'%s' isn't valid category name"), category);
    }

    return TRUE;
}

gboolean
cp_atom_slot_validate(const char *slot, GError **error) {
    /*@only@*/ static GRegex *regex = NULL;

    GError *tmp_error = NULL;

    g_assert(error == NULL || *error == NULL);

    if (regex == NULL) {
        /*@-mustfreeonly@*/
        regex = g_regex_new("^" SLOT "$", (int)G_REGEX_OPTIMIZE, 0, &tmp_error);
        /*@=mustfreeonly@*/
        g_assert_no_error(tmp_error);
        if (regex == NULL) {
            g_assert_not_reached();
        }
    }

    if (!g_regex_match_full(regex, slot, (gssize)-1, 0, 0, NULL, &tmp_error)) {
        g_assert_no_error(tmp_error);
        g_set_error(error, CP_ERROR, (gint)CP_ERROR_ATOM_SYNTAX,
            _("'%s' isn't valid slot"), slot);
        return FALSE;
    }

    return TRUE;
}

gboolean
cp_atom_repo_validate(const char *repo, GError **error) {
    /*@only@*/ static GRegex *regex = NULL;

    GError *tmp_error = NULL;

    g_assert(error == NULL || *error == NULL);

    if (regex == NULL) {
        /*@-mustfreeonly@*/
        regex = g_regex_new("^" REPO "$", (int)G_REGEX_OPTIMIZE, 0, &tmp_error);
        /*@=mustfreeonly@*/
        g_assert_no_error(tmp_error);
        if (regex == NULL) {
            g_assert_not_reached();
        }
    }

    if (!g_regex_match_full(regex, repo, (gssize)-1, 0, 0, NULL, &tmp_error)) {
        g_assert_no_error(tmp_error);
        g_set_error(error, CP_ERROR, (gint)CP_ERROR_ATOM_SYNTAX,
            _("'%s' isn't valid repository"), repo);
        return FALSE;
    }

    return TRUE;
}

gboolean
cp_atom_pv_split(const char *pv, char **name, CPVersion *version, GError **error) {
    /*@only@*/ static GRegex *regex = NULL;

    GError *tmp_error = NULL;

    gboolean result;
    GMatchInfo *match = NULL;
    char *ver_str = NULL;

    g_assert(error == NULL || *error == NULL);

    if (regex == NULL) {
        /*@-mustfreeonly@*/
        regex = g_regex_new("^" PV "$", (int)G_REGEX_OPTIMIZE, 0, &tmp_error);
        /*@=mustfreeonly@*/
        g_assert_no_error(tmp_error);
        if (regex == NULL) {
            g_assert_not_reached();
        }
    }

    result = g_regex_match_full(regex, pv, (gssize)-1, 0, 0, &match, &tmp_error);
    if (!result) {
        g_assert_no_error(tmp_error);
        g_set_error(error, CP_ERROR, (gint)CP_ERROR_ATOM_SYNTAX,
            _("'%s' isn't valid package-version"), pv);
        goto OUT;
    }

    result = check_invalid_version(match, 2, error);
    if (!result) {
        goto OUT;
    }

    ver_str = safe_fetch(match, 7);
    *version = cp_version_new(ver_str, error);
    if (*version == NULL) {
        goto OUT;
    }

    *name = safe_fetch(match, 1);

OUT:
    g_free(ver_str);
    g_match_info_free(match);
    return result;
}
