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

#include <string.h>

#include "package.h"
#include "version.h"

struct CPPackageS {
    /*@only@*/ char *category;
    /*@only@*/ char *name;
    CPVersion version;
    /*@only@*/ char *slot;
    /*@only@*/ char *subslot;
    /*@only@*/ char *repo;
    /*@only@*/ char *str;

    /*@refs@*/ unsigned int refs;
};

static void
init_slot(CPPackage self, const char *slot) {
    char **tokens = g_strsplit(slot, "/", 2);

    g_assert(strlen(slot) > 0);
    g_assert(self->slot == NULL);
    g_assert(self->subslot == NULL);

    self->slot = g_strdup(tokens[0]);
    self->subslot = g_strdup(tokens[1] == NULL ? self->slot : tokens[1]);
    g_strfreev(tokens);
}

CPPackage
cp_package_new(
    const char *category,
    const char *name,
    CPVersion version,
    const char *slot,
    const char *repo
) {
    CPPackage self;

    self = g_new0(struct CPPackageS, 1);
    self->refs = (unsigned int)1;

    /* TODO: validate args or make function private */
    g_assert(self->category == NULL);
    self->category = g_strdup(category);
    g_assert(self->name == NULL);
    self->name = g_strdup(name);
    g_assert(self->version == NULL);
    self->version = cp_version_ref(version);
    init_slot(self, slot);
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
    /*@-mustfreeonly@*/
    if (self == NULL) {
        return;
    }

    g_assert(self->refs > 0);
    if (--self->refs > 0) {
        return;
    }
    /*@=mustfreeonly@*/

    g_free(self->category);
    g_free(self->name);
    cp_version_unref(self->version);
    g_free(self->slot);
    g_free(self->subslot);
    g_free(self->repo);
    g_free(self->str);

    /*@-refcounttrans@*/
    g_free(self);
    /*@=refcounttrans@*/
}

void
cp_package_list_free(GSList *list) {
    g_slist_free_full(list, (GDestroyNotify)cp_package_unref);
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
cp_package_subslot(const CPPackage self) {
    return self->subslot;
}

const char *
cp_package_repo(const CPPackage self) {
    return self->repo;
}

int
cp_package_cmp(const CPPackage first, const CPPackage second) {
    int result;

    result = strcmp(first->category, second->category);
    if (result != 0) {
        return result;
    }

    result = strcmp(first->name, second->name);
    if (result != 0) {
        return result;
    }

    return cp_version_cmp(first->version, second->version);
}

const char *
cp_package_str(const CPPackage self) {
    return self->str;
}
