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

#include "version.h"

struct CPVersion {
    /*@only@*/ char *raw;

    /*@refs@*/ int refs;
};

CPVersion
cp_version_new(const char *version, GError **error) {
    CPVersion self;

    g_assert(error == NULL || *error == NULL);

    self = g_new0(struct CPVersion, 1);
    self->refs = 1;
    g_assert(self->raw == NULL);
    self->raw = g_strdup(version);

    return self;
}

CPVersion
cp_version_ref(CPVersion self) {
    ++self->refs;
    /*@-refcounttrans@*/
    return self;
    /*@=refcounttrans@*/
}

void
cp_version_unref(CPVersion self) {
    if (self == NULL) {
        /*@-mustfreeonly@*/
        return;
        /*@=mustfreeonly@*/
    }
    g_assert(self->refs > 0);
    if (--self->refs == 0) {
        g_free(self->raw);

        /*@-refcounttrans@*/
        g_free(self);
        /*@=refcounttrans@*/
    }
}

const char *
cp_version_str(const CPVersion self) {
    return self->raw;
}

int
cp_version_cmp(CPVersion first, CPVersion second) {
    /* TODO: implement */
    return g_strcmp0(first->raw, second->raw);
}

gboolean
cp_version_any_revision_match(
    /*@unused@*/ const CPVersion first G_GNUC_UNUSED,
    /*@unused@*/ const CPVersion second G_GNUC_UNUSED
) {
    /* TODO: implement */
    return TRUE;
}

gboolean
cp_version_glob_match(
    /*@unused@*/ const CPVersion first G_GNUC_UNUSED,
    /*@unused@*/ const CPVersion second G_GNUC_UNUSED
) {
    /* TODO: implement */
    return TRUE;
}
