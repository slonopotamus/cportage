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

#include <cportage/package.h>

struct CPPackage {
    char *category;
    char *name;
    char *version;
    char *slot;

    /*@refs@*/ int refs;
};

CPPackage
cp_package_new(
    const char *category,
    const char *name,
    const char *version,
    const char *slot
) {
    CPPackage self;

    self = g_new0(struct CPPackage, 1);
    self->refs = 1;

    /* TODO: validate args */
    g_assert(self->category == NULL);
    self->category = g_strdup(category);
    g_assert(self->name == NULL);
    self->name = g_strdup(name);
    g_assert(self->version == NULL);
    self->version = g_strdup(version);
    g_assert(self->slot == NULL);
    self->slot = g_strdup(slot);

    return self;
}

CPPackage
cp_package_ref(CPPackage self) {
    ++self->refs;
    /*@-refcounttrans@*/
    return self;
    /*@=refcounttrans@*/
}

void
cp_package_unref(CPPackage self) {
    if (self == NULL) {
        /*@-mustfreeonly@*/
        return;
        /*@=mustfreeonly@*/
    }
    g_assert(self->refs > 0);
    if (--self->refs == 0) {
        g_free(self->category);
        g_free(self->name);
        g_free(self->version);
        g_free(self->slot);

        /*@-refcounttrans@*/
        g_free(self);
        /*@=refcounttrans@*/
    }
}

void
cp_package_list_free(GList *list) {
#if GLIB_CHECK_VERSION(2,27,2)
    g_list_free_full(list, (GDestroyNotify)cp_package_unref);
#else
    g_list_foreach(list, (GFunc)cp_package_unref, NULL);
    g_list_free(list);
#endif
}

const char *
cp_package_category(const CPPackage self) {
    return self->category;
}
const char *
cp_package_name(const CPPackage self) {
    return self->name;
}

const char *
cp_package_version(const CPPackage self) {
    return self->version;
}

const char *
cp_package_slot(const CPPackage self) {
    return self->slot;
}
