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

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <cportage.h>

#include "path.h"

char *
cp_path_realpath(const char *path, GError **error) {
    char *result;

    g_assert(error == NULL || *error == NULL);

    /* TODO: alternative impl using glibc canonicalize_file_name(3)? */
    /*@-unrecog@*/
    result = realpath(path, NULL);
    /*@=unrecog@*/

    if (result == NULL) {
        int save_errno = errno;
        g_set_error(error, G_FILE_ERROR, g_file_error_from_errno(save_errno),
            _("Can't get real path of '%s': %s"),
            path, g_strerror(save_errno));
    }

    return result;
}

/** Taken from glocalfile.c */
static /*@null@*/ const char *
match_prefix(/*@returned@*/ const char *path, const char *prefix) /*@*/ {
    size_t prefix_len = strlen(prefix);
    if (strncmp(path, prefix, prefix_len) != 0) {
      return NULL;
    }

    /* Handle the case where prefix is the root, so that
     * the IS_DIR_SEPRARATOR check below works */
    if (prefix_len > 0 && G_IS_DIR_SEPARATOR(prefix[prefix_len-1])) {
      prefix_len--;
    }

    return &path[prefix_len];
}

char *
cp_path_get_relative(const char *parent, const char *descendant) {
    const char *remainder = match_prefix(descendant, parent);

    if (remainder == NULL || !G_IS_DIR_SEPARATOR(remainder[0])) {
        return NULL;
    }

    return g_strdup(&remainder[1]);
}

gboolean
cp_path_is_descendant(const char *parent, const char *descendant) {
    const char *remainder = match_prefix(descendant, parent);
    return remainder != NULL && G_IS_DIR_SEPARATOR(*remainder);
}
