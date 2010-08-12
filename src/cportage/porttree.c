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

struct CPPorttree {
    /*@refs@*/ int refs;
    CPSettings settings;
};

CPPorttree
cp_porttree_new(CPSettings settings) {
    CPPorttree self = g_new(struct CPPorttree, 1);
    self->refs = 1;
    self->settings = cp_settings_ref(settings);
    return self;
}

CPPorttree
cp_porttree_ref(CPPorttree self) {
    ++self->refs;
    return self;
}

void
cp_porttree_unref(CPPorttree self) {
    if (self == NULL) {
        /*@-mustfreeonly@*/
        return;
        /*@=mustfreeonly@*/
    }
    g_assert(self->refs > 0);
    if (--self->refs == 0) {
        cp_settings_unref(self->settings);
        /*@-refcounttrans@*/
        g_free(self);
        /*@=refcounttrans@*/
    }
}
