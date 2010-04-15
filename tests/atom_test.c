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

#include "cportage/atom.h"

struct item {
    const char * str;
    const bool valid;
};

int main(void) {
    const struct item data[] = {
        { "sys-apps/portage", true },
        { "=sys-apps/portage-2.1", true },
        { "=sys-apps/portage-2.1*", true },
        { ">=sys-apps/portage-2.1", true },
        { "<=sys-apps/portage-2.1", true },
        { ">sys-apps/portage-2.1", true },
        { "<sys-apps/portage-2.1", true },
        { "~sys-apps/portage-2.1", true },
        { "sys-apps/portage:foo", true },
        { "sys-apps/portage-2.1:foo", false },
        { "sys-apps/portage-2.1:", false },
        { ">~cate-gory/foo-1.0", false },
        { ">~category/foo-1.0", false },
        { "<~category/foo-1.0", false },
        { "###cat/foo-1.0", false },
        { "~sys-apps/portage", false },
        { "portage", false },
        { "=portage", false },
        { ">=portage-2.1", false },
        { "~portage-2.1", false },
        { "=portage-2.1*", false },
        { "null/portage", true },
        { "null/portage*:0", false },
        { ">=null/portage-2.1", true },
        { ">=null/portage", false },
        { ">null/portage", false },
        { "=null/portage*", false },
        { "=null/portage", false },
        { "~null/portage", false },
        { "<=null/portage", false },
        { "<null/portage", false },
        { "~null/portage-2.1", true },
        { "=null/portage-2.1*", true },
        { "null/portage-2.1*", false },
        { "app-doc/php-docs-20071125", false },
        { "app-doc/php-docs-20071125-r2", false },
        { "=foo/bar-1-r1-1-r1", false },
        { "foo/-z-1", false },

        /*
            These are invalid because pkg name must not end in hyphen
            followed by version number
         */
        { "=foo/bar-1-r1-1-r1", false },
        { "=foo/bar-123-1", false },
        { "=foo/bar-123-1*", false },
        { "foo/bar-123", false },
        { "=foo/bar-123-1-r1", false },
        { "=foo/bar-123-1-r1*", false },
        { "foo/bar-123-r1", false },
        { "foo/bar-1", false },

        { "=foo/bar--baz-1-r1", true },
        { "=foo/bar-baz--1-r1", true },
        { "=foo/bar-baz---1-r1", true },
        { "=foo/bar-baz---1", true },
        { "=foo/bar-baz-1--r1", false },
        { "games-strategy/ufo2000", true },
        { "~games-strategy/ufo2000-0.1", true },
        { "=media-libs/x264-20060810", true },
        { "foo/b", true },
        { "app-text/7plus", true },
        { "foo/666", true },
        { "=dev-libs/poppler-qt3-0.11*", true },

        { "sys-apps/portage[foo]", true },
        { "sys-apps/portage-2.1:[foo]", false },
        { "=sys-apps/portage-2.2*:foo[bar?,!baz?,!doc=,build=]", true },
        { "=sys-apps/portage-2.2*:foo[doc?]", true },
        { "=sys-apps/portage-2.2*:foo[!doc?]", true },
        { "=sys-apps/portage-2.2*:foo[doc=]", true },
        { "=sys-apps/portage-2.2*:foo[!doc=]", true },
        { "=sys-apps/portage-2.2*:foo[!doc]", false },
        { "=sys-apps/portage-2.2*:foo[!-doc]", false },
        { "=sys-apps/portage-2.2*:foo[!-doc=]", false },
        { "=sys-apps/portage-2.2*:foo[!-doc?]", false },
        { "=sys-apps/portage-2.2*:foo[-doc?]", false },
        { "=sys-apps/portage-2.2*:foo[-doc=]", false },
        { "=sys-apps/portage-2.2*:foo[-doc!=]", false },
        { "=sys-apps/portage-2.2*:foo[-doc=]", false },
        { "=sys-apps/portage-2.2*:foo[bar][-baz][doc?][!build?]", false },
        { "=sys-apps/portage-2.2*:foo[bar,-baz,doc?,!build?]", true },
        { "=sys-apps/portage-2.2*:foo[bar,-baz,doc?,!build?,]", false },
        { "=sys-apps/portage-2.2*:foo[,bar,-baz,doc?,!build?]", false },
        { "=sys-apps/portage-2.2*:foo[bar,-baz][doc?,!build?]", false },
        { "=sys-apps/portage-2.2*:foo[bar][doc,build]", false }
    };

    size_t i = 0;
    while (i < G_N_ELEMENTS(data)) {
        const char * s = data[i].str;
        const char * msg = data[i].valid ? "valid" : "invalid";
        void *atom = cportage_atom_new(s, NULL);
        if (data[i].valid == (atom == NULL)) {
            g_error("'%s' must be %s, but it isn't\n", s, msg);
        }
        cportage_atom_unref(atom);
        ++i;
    }
    return EXIT_SUCCESS;
}
