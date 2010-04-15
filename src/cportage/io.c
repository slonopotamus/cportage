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

#include <errno.h>
#include <glib/gi18n-lib.h>
#include <stdlib.h>

#include "cportage/io.h"

char *
cportage_canonical_path(char *path, GError **error) {
    char *path_enc;
    char *result;

    g_assert(error == NULL || *error == NULL);
    g_assert(g_utf8_validate(path, -1, NULL));

    if ((path_enc = g_filename_from_utf8(path, -1, NULL, NULL, error)) == NULL) {
        result = NULL;
    } else {
        /* TODO: altertative impl using glibc canonicalize_path? */
        char *result_enc = realpath(path_enc, NULL);
        if (result_enc == NULL) {
            int save_errno = errno;
            g_set_error (error,
                G_FILE_ERROR,
                g_file_error_from_errno (save_errno),
                _("Failed to canonicalize file '%s': realpath() failed: %s"),
                path, 
		            g_strerror (save_errno));
		        result = NULL;
        } else {
            result = g_filename_to_utf8(result_enc, -1, NULL, NULL, error);
        }
        free(result_enc);
    }

    g_free(path_enc);
    g_assert(g_utf8_validate(result, -1, NULL));
    return result;
}

char **
cportage_read_lines(const char *path, const bool ignore_comments, GError **error) {
    char *path_enc = NULL;
    char *data = NULL;
    char **result;

    g_assert(error == NULL || *error == NULL);
    g_assert(g_utf8_validate(path, -1, NULL));

    if ((path_enc = g_filename_from_utf8(path, -1, NULL, NULL, error)) == NULL) {
        result = NULL;
    } else if (!g_file_get_contents(path_enc, &data, NULL, error)) {
        result = NULL;
    } else if (g_utf8_validate(data, -1, NULL)) {
        char **lines = g_strsplit(data, "\n", -1);
        if (ignore_comments) {
            char *line;
            int i = 0;
            int j = 0;
            result = g_new(char *, g_strv_length(lines) + 1);
            while ((line = lines[i++]) != NULL) {
                char *comment = g_utf8_strchr(line, -1, '#');
                if (comment != NULL) {
                    *comment = '\0';
                }
                (void)g_strstrip(line);
                if (line[0] != '\0') {
                    result[j++] = g_strdup(line);
                }
            }
            result[j] = NULL;
            g_strfreev(lines);
        } else {
            result = lines;
        }
    } else {
        g_set_error(error, G_CONVERT_ERROR,
            G_CONVERT_ERROR_ILLEGAL_SEQUENCE,
            _("Invalid byte sequence in %s"), path);
        result = NULL;
    }

    g_free(path_enc);
    g_free(data);
    return result;
}

