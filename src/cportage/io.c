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
#include <glib/gstdio.h>
#include <stdlib.h>

#include <cportage/io.h>
#include <cportage/strings.h>

static gboolean
cp_utf8_validate(const char *str, const char *file, GError **error) {
    g_assert(error == NULL || *error == NULL);

    if (!g_utf8_validate(str, -1, NULL)) {
        /* TODO: report where we got invalid byte sequence exactly */
        g_set_error(error, G_CONVERT_ERROR, G_CONVERT_ERROR_ILLEGAL_SEQUENCE,
            _("Invalid UTF-8 byte sequence in '%s'"), file);
        return FALSE;
    }

    return TRUE;
}

FILE *
cp_fopen(const char *path, const char *mode, GError **error) {
    char *path_enc;
    FILE *result = NULL;

    g_assert(error == NULL || *error == NULL);
    g_assert(g_utf8_validate(path, -1, NULL));

    path_enc = g_filename_from_utf8(path, -1, NULL, NULL, error);
    if (path_enc == NULL) {
        goto ERR;
    }

    result = g_fopen(path_enc, mode);
    if (result == NULL) {
        int save_errno = errno;
        /*@-type@*/
        int error_code = g_file_error_from_errno(save_errno);
        /*@=type@*/
        g_set_error(error, G_FILE_ERROR, error_code,
            _("Can't open '%s': %s"),
            path, g_strerror(save_errno));
    }

ERR:
    g_free(path_enc);

    return result;
}

int
cp_getline(FILE *stream, const char *stream_desc, char **into, GError **error) {
    GString *string;
    int c;

    g_assert(error == NULL || *error == NULL);

    string = g_string_new("");

    while ((c = fgetc(stream)) != EOF) {
        g_string_append_c(string, (unsigned char)c);
        if (c == '\n') {
            break;
        }
    }

    if (ferror(stream)) {
        int save_errno = errno;
        /*@-type@*/
        int error_code = g_file_error_from_errno(save_errno);
        /*@=type@*/
        g_set_error(error, G_FILE_ERROR, error_code,
            _("Error reading from '%s': %s"),
            stream_desc, g_strerror(save_errno));
        goto ERR;
    }

    if (string->len < 1) {
        g_string_free(string, TRUE);
        return 0;
    }

    if (!cp_utf8_validate(string->str, stream_desc, error)) {
        goto ERR;
    }

    *into = string->str;
    g_string_free(string, FALSE);
    return 1;

ERR:
    g_string_free(string, TRUE);
    return -1;
}

char *
cp_canonical_path(const char *path, GError **error) {
    char *path_enc;
    char *result_enc;
    char *result = NULL;

    g_assert(error == NULL || *error == NULL);
    g_assert(g_utf8_validate(path, -1, NULL));

    path_enc = g_filename_from_utf8(path, -1, NULL, NULL, error);
    if (path_enc == NULL) {
        goto ENC_ERR;
    }

    /* TODO: alternative impl using glibc canonicalize_file_name(3)? */
    /*@-unrecog@*/
    result_enc = realpath(path_enc, NULL);
    /*@=unrecog@*/

    if (result_enc == NULL) {
        int save_errno = errno;
        /*@-type@*/
        int error_code = g_file_error_from_errno(save_errno);
        /*@=type@*/
        g_set_error(error, G_FILE_ERROR, error_code,
            _("Can't get real path of '%s': %s"),
            path, g_strerror(save_errno));
        goto FS_ERR;
    }

    result = g_filename_to_utf8(result_enc, -1, NULL, NULL, error);

FS_ERR:
    free(result_enc);
ENC_ERR:
    g_free(path_enc);

    return result;
}

gboolean
cp_read_file(const char *path, char **data, size_t *len, GError **error) {
    char *path_enc;
    gboolean result;

    g_assert(error == NULL || *error == NULL);
    g_assert(g_utf8_validate(path, -1, NULL));

    path_enc = g_filename_from_utf8(path, -1, NULL, NULL, error);
    result = path_enc != NULL && g_file_get_contents(path_enc, data, len, error);

    g_free(path_enc);
    return result;
}

/**
    TODO: current impl temporarily allocs 3x size of file.
    Could be rewritten to 1x.
 */
char **
cp_read_lines(const char *path, const gboolean ignore_comments, GError **error) {
    char *data = NULL;
    char **result = NULL;

    g_assert(error == NULL || *error == NULL);

    if (!cp_read_file(path, &data, NULL, error)) {
        goto ERR;
    }

    if (!cp_utf8_validate(data, path, error)) {
        goto ERR;
    }

    if (ignore_comments) {
        char **lines = g_strsplit(data, "\n", -1);
        char *line;
        size_t i = 0;
        size_t j = 0;
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
        result = g_strsplit(data, "\n", -1);
    }

ERR:
    g_free(data);
    return result;
}
