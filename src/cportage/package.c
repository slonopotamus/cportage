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

#include "package.h"
#include "version.h"

struct CPPackage {
    /*@only@*/ char *category;
    /*@only@*/ char *name;
    CPVersion version;
    /*@only@*/ char *slot;
    /*@only@*/ char *repo;
    /*@only@*/ char *str;

    /*@refs@*/ int refs;
};

CPPackage
cp_package_new(
    const char *category,
    const char *name,
    CPVersion version,
    const char *slot,
    const char *repo
) {
    CPPackage self;

    self = g_new0(struct CPPackage, 1);
    self->refs = 1;

    /* TODO: validate args or make function private */
    g_assert(self->category == NULL);
    self->category = g_strdup(category);
    g_assert(self->name == NULL);
    self->name = g_strdup(name);
    g_assert(self->version == NULL);
    self->version = cp_version_ref(version);
    g_assert(self->slot == NULL);
    self->slot = g_strdup(slot);
    g_assert(self->repo == NULL);
    self->repo = g_strdup(repo);
    g_assert(self->str == NULL);
    self->str = g_strdup_printf("%s/%s-%s", category, name, cp_version_str(version));

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
        cp_version_unref(self->version);
        g_free(self->slot);
        g_free(self->repo);
        g_free(self->str);

        /*@-refcounttrans@*/
        g_free(self);
        /*@=refcounttrans@*/
    }
}

void
cp_package_list_free(GList *list) {
    g_list_free_full(list, (GDestroyNotify)cp_package_unref);
}

const char *
cp_package_category(const CPPackage self) {
    return self->category;
}
const char *
cp_package_name(const CPPackage self) {
    return self->name;
}

CPVersion
cp_package_version(const CPPackage self) {
    return cp_version_ref(self->version);
}

const char *
cp_package_slot(const CPPackage self) {
    return self->slot;
}

const char *
cp_package_repo(const CPPackage self) {
    return self->repo;
}

int
cp_package_cmp(const CPPackage first, const CPPackage second) {
    int result;

    result = g_strcmp0(first->category, second->category);
    if (result != 0) {
        return result;
    }

    result = g_strcmp0(first->name, second->name);
    if (result != 0) {
        return result;
    }

    return cp_version_cmp(first->version, second->version);
}

const char *
cp_package_str(const CPPackage self) {
    return self->str;
}
