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

#include <cportage/repository.h>

struct CPRepository {
    /*@only@*/ char *path;

    /*@refs@*/ int refs;
};

CPRepository
cp_repository_new(const char *path) {
    CPRepository self;

    self = g_new0(struct CPRepository, 1);
    self->refs = 1;
    self->path = g_strdup(path);

    return self;
}

CPRepository
cp_repository_ref(CPRepository self) {
    ++self->refs;
    return self;
}

void
cp_repository_unref(CPRepository self) {
    if (self == NULL) {
        /*@-mustfreeonly@*/
        return;
        /*@=mustfreeonly@*/
    }
    g_assert(self->refs > 0);
    if (--self->refs == 0) {
        g_free(self->path);

        /*@-refcounttrans@*/
        g_free(self);
        /*@=refcounttrans@*/
    }
}

gboolean
cp_repository_sync(const CPRepository self, GError **error) {
    gboolean result;
    char **sync_cmd;
    int exit_status;

    g_assert(error == NULL || *error == NULL);

    sync_cmd = g_strsplit("git pull", " ", 0);
    g_message(">>> Starting git pull in %s...", self->path);
    /** TODO: path encoding */
    result = g_spawn_sync(self->path, sync_cmd, NULL, G_SPAWN_SEARCH_PATH,
        NULL, NULL, NULL, NULL, &exit_status, error);
    g_strfreev(sync_cmd);

    if (result && exit_status == EXIT_SUCCESS) {
        g_message(">>> git pull in %s successful", self->path);
    } else {
        g_message("!!! git pull error in %s", self->path);
        /** TODO: set error if result == TRUE */
    }

    return result;
}

const char *
cp_repository_get_path(const CPRepository self) {
    return self->path;
}
