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

#include <stdlib.h>

#include "cportage/porttree.h"

struct CPortagePorttree {
    /*@refs@*/ int refs;
    /*@refcounted@*/ CPortageSettings settings;
};

CPortagePorttree
cportage_porttree_new(CPortageSettings settings) {
    CPortagePorttree self = g_malloc0(sizeof(*self));
    self->refs = 1;
    self->settings = cportage_settings_ref(settings);
    return self;
}

CPortagePorttree
cportage_porttree_ref(CPortagePorttree self) {
    ++self->refs;
    return self;
}

void
cportage_porttree_unref(CPortagePorttree self) {
    g_return_if_fail(self != NULL);
    g_assert(self->refs > 0);
    if (--self->refs == 0) {
        cportage_settings_unref(self->settings);
        /*@-refcounttrans@*/
        free(self);
        /*@=refcounttrans@*/
    }
}

char *
cportage_porttree_get_path(const CPortagePorttree self, const char *relative) {
    char *portdir = cportage_settings_get_portdir(self->settings);
    char *result = g_build_filename(portdir, relative, NULL);
    free(portdir);
    return result;
}
