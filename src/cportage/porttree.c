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

#include "cportage/porttree.h"

struct CPortagePorttree {
    /*@refs@*/ int refs;
    CPortageSettings settings;
};

CPortagePorttree
cportage_porttree_new(CPortageSettings settings) {
    CPortagePorttree self = g_new(struct CPortagePorttree, 1);
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
    if (self == NULL) {
        /*@-mustfreeonly@*/
        return;
        /*@=mustfreeonly@*/
    }
    g_assert_cmpint(self->refs, >, 0);
    if (--self->refs == 0) {
        cportage_settings_unref(self->settings);
        /*@-refcounttrans@*/
        g_free(self);
        /*@=refcounttrans@*/
    }
}
