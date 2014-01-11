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

#include <stdlib.h>

#include "repository.h"

struct CPRepositoryS {
    /*@only@*/ char *name;
    /*@only@*/ char *path;

    /*@refs@*/ unsigned int refs;
};

static char * G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT
read_repo_name(
    const char *repo_path
) /*@modifies *stderr,errno@*/ /*@globals fileSystem@*/ {
    /*@only@*/ char *result = NULL;
    char *path = g_build_filename(repo_path, "profiles", "repo_name", NULL);
    /*@-modfilesys@*/
    FILE *f = cp_io_fopen(path, "r", NULL);
    /*@=modfilesys@*/

    /* TODO: validate repo name */
    if (f == NULL || cp_io_getline(f, path, &result, NULL) <= 0) {
        char *basename = g_path_get_basename(repo_path);
        result = g_strconcat("x-", basename, NULL);
        g_free(basename);
        g_debug(_("Repository '%s' is missing 'profiles/repo_name' file,"
          " using '%s' as repository name"), repo_path, result);
    } else {
        g_assert(result != NULL);
        (void)g_strstrip(result);
    }

    if (f != NULL) {
        /*@-modfilesys@*/
        (void)fclose(f);
        /*@=modfilesys@*/
    }

    g_free(path);
    return result;
}

CPRepository
cp_repository_new(const char *path) {
    CPRepository self;

    self = g_new0(struct CPRepositoryS, 1);
    self->refs = (unsigned int)1;
    g_assert(self->path == NULL);
    self->path = g_strdup(path);
    g_assert(self->name == NULL);
    self->name = read_repo_name(path);

    return self;
}

CPRepository
cp_repository_ref(CPRepository self) {
    ++self->refs;
    /*@-refcounttrans@*/
    return self;
    /*@=refcounttrans@*/
}

void
cp_repository_unref(CPRepository self) {
    /*@-mustfreeonly@*/
    if (self == NULL) {
        return;
    }

    g_assert(self->refs > 0);
    if (--self->refs > 0) {
        return;
    }
    /*@=mustfreeonly@*/

    g_free(self->name);
    g_free(self->path);

    /*@-refcounttrans@*/
    g_free(self);
    /*@=refcounttrans@*/
}

int
cp_repository_sync(const CPRepository self, GError **error) {
    gboolean result;
    char **sync_cmd;
    int status;

    g_assert(error == NULL || *error == NULL);

    sync_cmd = g_strsplit("git pull", " ", 0);
    g_message(_("Starting git pull in %s..."), self->path);
    result = g_spawn_sync(self->path, sync_cmd, NULL, (gint)G_SPAWN_SEARCH_PATH,
        NULL, NULL, NULL, NULL, &status, error);
    g_strfreev(sync_cmd);

    if (!result) {
        return EXIT_FAILURE;
    }

    if (WIFEXITED(status) != 0) {
        g_set_error(error, G_SPAWN_ERROR, (gint)G_SPAWN_ERROR_FAILED,
            _("git pull error in %s"), self->path);
        return EXIT_FAILURE;
    }

    if (WEXITSTATUS(status) != EXIT_SUCCESS) {
        g_set_error(error, G_SPAWN_ERROR, (gint)G_SPAWN_ERROR_FAILED,
            _("git pull error in %s"), self->path);
        return WEXITSTATUS(status);
    }

    g_message(_("git pull in %s successful"), self->path);
    return EXIT_SUCCESS;
}

const char *
cp_repository_name(const CPRepository self) {
    return self->name;
}

const char *
cp_repository_path(const CPRepository self) {
    return self->path;
}
