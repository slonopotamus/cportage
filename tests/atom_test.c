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

#include <stdlib.h>

#include "cportage/atom.h"

struct item {
    const char *str;
    const gboolean valid;
};

int main(void) {
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
        void *atom = cp_atom_new(s, NULL);
        if (data[i].valid == (atom == NULL)) {
            g_error("'%s' must be %s, but it isn't\n", s, msg);
        }
        cp_atom_unref(atom);
        ++i;
    }

    return EXIT_SUCCESS;
}
