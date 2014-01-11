/*
    Copyright 2009-2014, Marat Radchenko

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

#include <stdlib.h>

#include <cportage.h>
#include "cportage/atom.h"

struct item {
    const char *str;
    const gboolean valid;
};

static void
atom_new(void) {
    CPAtomFactory atom_factory = cp_atom_factory_new();

    const struct item data[] = {
        { "sys-apps/portage", TRUE },
        { "=sys-apps/portage-2.1", TRUE },
        { "=sys-apps/portage-2.1*", TRUE },
        { ">=sys-apps/portage-2.1", TRUE },
        { "<=sys-apps/portage-2.1", TRUE },
        { ">sys-apps/portage-2.1", TRUE },
        { "<sys-apps/portage-2.1", TRUE },
        { "~sys-apps/portage-2.1", TRUE },
        { "sys-apps/portage:foo", TRUE },
        { "sys-apps/portage-2.1:foo", FALSE },
        { "sys-apps/portage-2.1:", FALSE },
        { ">~cate-gory/foo-1.0", FALSE },
        { ">~category/foo-1.0", FALSE },
        { "<~category/foo-1.0", FALSE },
        { "###cat/foo-1.0", FALSE },
        { "~sys-apps/portage", FALSE },
        { "portage", FALSE },
        { "=portage", FALSE },
        { ">=portage-2.1", FALSE },
        { "~portage-2.1", FALSE },
        { "=portage-2.1*", FALSE },
        { "null/portage", TRUE },
        { "null/portage*:0", FALSE },
        { ">=null/portage-2.1", TRUE },
        { ">=null/portage", FALSE },
        { ">null/portage", FALSE },
        { "=null/portage*", FALSE },
        { "=null/portage", FALSE },
        { "~null/portage", FALSE },
        { "<=null/portage", FALSE },
        { "<null/portage", FALSE },
        { "~null/portage-2.1", TRUE },
        { "=null/portage-2.1*", TRUE },
        { "null/portage-2.1*", FALSE },
        { "app-doc/php-docs-20071125", FALSE },
        { "app-doc/php-docs-20071125-r2", FALSE },
        { "=foo/bar-1-r1-1-r1", FALSE },
        { "foo/-z-1", FALSE },

        /*
            These are invalid because pkg name must not end in hyphen
            followed by version number
         */
        { "=foo/bar-1-r1-1-r1", FALSE },
        { "=foo/bar-123-1", FALSE },
        { "=foo/bar-123-1*", FALSE },
        { "foo/bar-123", FALSE },
        { "=foo/bar-123-1-r1", FALSE },
        { "=foo/bar-123-1-r1*", FALSE },
        { "foo/bar-123-r1", FALSE },
        { "foo/bar-1", FALSE },

        { "=foo/bar--baz-1-r1", TRUE },
        { "=foo/bar-baz--1-r1", TRUE },
        { "=foo/bar-baz---1-r1", TRUE },
        { "=foo/bar-baz---1", TRUE },
        { "=foo/bar-baz-1--r1", FALSE },
        { "games-strategy/ufo2000", TRUE },
        { "~games-strategy/ufo2000-0.1", TRUE },
        { "=media-libs/x264-20060810", TRUE },
        { "foo/b", TRUE },
        { "app-text/7plus", TRUE },
        { "foo/666", TRUE },
        { "=dev-libs/poppler-qt3-0.11*", TRUE },

        /* EAPI-5 subslot */
        { "dev-lang/spidermonkey:0/mozjs185", TRUE },

        { "sys-apps/portage[foo]", TRUE },
        { "sys-apps/portage-2.1:[foo]", FALSE },
        { "=sys-apps/portage-2.2*:foo[bar?,!baz?,!doc=,build=]", TRUE },
        { "=sys-apps/portage-2.2*:foo[doc?]", TRUE },
        { "=sys-apps/portage-2.2*:foo[!doc?]", TRUE },
        { "=sys-apps/portage-2.2*:foo[doc=]", TRUE },
        { "=sys-apps/portage-2.2*:foo[!doc=]", TRUE },
        { "=sys-apps/portage-2.2*:foo[!doc]", FALSE },
        { "=sys-apps/portage-2.2*:foo[!-doc]", FALSE },
        { "=sys-apps/portage-2.2*:foo[!-doc=]", FALSE },
        { "=sys-apps/portage-2.2*:foo[!-doc?]", FALSE },
        { "=sys-apps/portage-2.2*:foo[-doc?]", FALSE },
        { "=sys-apps/portage-2.2*:foo[-doc=]", FALSE },
        { "=sys-apps/portage-2.2*:foo[-doc!=]", FALSE },
        { "=sys-apps/portage-2.2*:foo[-doc=]", FALSE },
        { "=sys-apps/portage-2.2*:foo[bar][-baz][doc?][!build?]", FALSE },
        { "=sys-apps/portage-2.2*:foo[bar,-baz,doc?,!build?]", TRUE },
        { "=sys-apps/portage-2.2*:foo[bar,-baz,doc?,!build?,]", FALSE },
        { "=sys-apps/portage-2.2*:foo[,bar,-baz,doc?,!build?]", FALSE },
        { "=sys-apps/portage-2.2*:foo[bar,-baz][doc?,!build?]", FALSE },
        { "=sys-apps/portage-2.2*:foo[bar][doc,build]", FALSE },
    };

    size_t i = 0;
    while (i < G_N_ELEMENTS(data)) {
        const char *s = data[i].str;
        const char *msg = data[i].valid ? "valid" : "invalid";
        GError *error = NULL;
        CPAtom atom = cp_atom_new(atom_factory, CP_EAPI_LATEST, s, &error);
        if (data[i].valid == (atom == NULL)) {
            g_error("'%s' must be %s, but it isn't\n", s, msg);
        }
        if (data[i].valid != (error == NULL)) {
            g_error("Expected %s error for '%s' but got: %s",
                data[i].valid ? "no" : "an", s, error ? error->message : "NULL");
        }
        cp_atom_unref(atom);
        g_clear_error(&error);
        ++i;
    }

    cp_atom_factory_unref(atom_factory);
}

static void
pv_split(void) {
    char *pkg = NULL;
    CPVersion version = NULL;
    GError *error = NULL;
    gboolean success = cp_atom_pv_split("foo-1.0-r1", &pkg, &version, &error);

    g_assert_no_error(error);
    g_assert(success);
    g_assert_cmpstr(pkg, ==, "foo");
    g_assert(version != NULL);
    g_assert_cmpstr(cp_version_str(version), ==, "1.0-r1");
    g_free(pkg);
    cp_version_unref(version);
}

int main(int argc, char *argv[]) {
    g_test_init(&argc, &argv, NULL);

    g_test_add_func("/atom/new", atom_new);
    g_test_add_func("/atom/pv_split", pv_split);

    return g_test_run();
}
