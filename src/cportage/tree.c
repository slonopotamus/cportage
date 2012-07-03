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

#include <cportage.h>

struct CPTreeS {
    /*@owned@*/ void *priv;
    /*@shared@*/ CPTreeMethods methods;

    /*@refs@*/ unsigned int refs;
};

CPTree
cp_tree_new(const CPTreeMethods methods, void *priv) {
    CPTree self = g_new0(struct CPTreeS, 1);

    self->methods = methods;
    g_assert(self->priv == NULL);
    self->priv = priv;

    return self;
}

CPTree
cp_tree_ref(CPTree self) {
    ++self->refs;
    /*@-refcounttrans@*/
    return self;
    /*@=refcounttrans@*/
}

void
cp_tree_unref(CPTree self) {
    /*@-mustfreeonly@*/
    if (self == NULL) {
        return;
    }

    g_assert(self->refs > 0);
    if (--self->refs > 0) {
        return;
    }
    /*@=mustfreeonly@*/

    self->methods->destructor(self->priv);

    /*@-refcounttrans@*/
    g_free(self);
    /*@=refcounttrans@*/
}

gboolean
cp_tree_find_packages(
    CPTree self,
    const CPAtom atom,
    GSList **match,
    GError **error
) {
    g_assert(error == NULL || *error == NULL);

    return self->methods->find_packages(self->priv, atom, match, error);
}
