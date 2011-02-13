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

#include <glib/gstdio.h>

#include <cportage/io.h>
#include <cportage/strings.h>

FILE *
cp_io_fopen(const char *path, const char *mode, GError **error) {
    FILE *result = NULL;

    g_assert(error == NULL || *error == NULL);

    result = g_fopen(path, mode);
    if (result == NULL) {
        int save_errno = errno;
        g_set_error(error, G_FILE_ERROR, g_file_error_from_errno(save_errno),
            _("Can't open '%s': %s"), path, g_strerror(save_errno));
    }

    return result;
}

int
cp_io_getline(FILE *file, const char *file_desc, char **into, GError **error) {
    /*@only@*/ GString *line;
    int c;

    g_assert(error == NULL || *error == NULL);

    line = g_string_new("");

    /*@-modfilesys@*/
    while ((c = fgetc(file)) != EOF) {
        line = g_string_append_c(line, (gchar)(unsigned char)c);
        if (c == (int)'\n') {
            break;
        }
    }
    /*@=modfilesys@*/

    if (ferror(file) != 0) {
        int save_errno = errno;
        g_set_error(error, G_FILE_ERROR, g_file_error_from_errno(save_errno),
            _("Error reading from '%s': %s"),
            file_desc, g_strerror(save_errno));
        goto ERR;
    }

    if (line->len == 0) {
        *into = g_string_free(line, TRUE);
        return 0;
    }

    *into = g_string_free(line, FALSE);
    return 1;

ERR:
    /*@-usereleased@*/
    /*@-mustfreefresh*/
    *into = g_string_free(line, TRUE);
    /*@=mustfreefresh*/
    /*@=usereleased@*/
    return -1;
}

char *
cp_io_realpath(const char *path, GError **error) {
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

/**
    TODO: current impl temporarily allocs 3x size of file.
    Could be rewritten to 1x.
 */
char **
cp_io_getlines(const char *path, const gboolean ignore_comments, GError **error) {
    char *data = NULL;
    char **result = NULL;

    g_assert(error == NULL || *error == NULL);

    if (!g_file_get_contents(path, &data, NULL, error)) {
        goto ERR;
    }

    g_assert(data != NULL);

    if (ignore_comments) {
        char **lines = g_strsplit(data, "\n", -1);
        char *line;
        size_t i = 0;
        size_t j = 0;
        result = g_new(char *, g_strv_length(lines) + 1);
        while ((line = lines[i++]) != NULL) {
            char *comment = g_utf8_strchr(line, (gssize)-1, (gunichar)'#');
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
        result = g_strsplit(data, "\n", -1);
    }

ERR:
    g_free(data);
    return result;
}
