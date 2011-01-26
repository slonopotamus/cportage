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

#include <cportage/io.h>
#include <cportage/strings.h>

static gboolean G_GNUC_WARN_UNUSED_RESULT
cp_utf8_validate(
    const char *str,
    const char *file,
    /*@null@*/ GError **error
) /*@modifies *error@*/ {
    g_assert(error == NULL || *error == NULL);

    if (!g_utf8_validate(str, (gssize)-1, NULL)) {
        /* TODO: report where we got invalid byte sequence exactly */
        g_set_error(error, G_CONVERT_ERROR,
            (gint)G_CONVERT_ERROR_ILLEGAL_SEQUENCE,
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

    path_enc = g_filename_from_utf8(path, (gssize)-1, NULL, NULL, error);
    if (path_enc == NULL) {
        goto ERR;
    }

    result = fopen(path_enc, mode);
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
cp_getline(FILE *file, const char *file_desc, char **into, GError **error) {
    /*@only@*/ GString *line;
    int c;

    g_assert(error == NULL || *error == NULL);

    line = g_string_new("");

    while ((c = fgetc(file)) != EOF) {
        line = g_string_append_c(line, (gchar)(unsigned char)c);
        if (c == '\n') {
            break;
        }
    }

    if (ferror(file) != 0) {
        int save_errno = errno;
        /*@-type@*/
        int error_code = g_file_error_from_errno(save_errno);
        /*@=type@*/
        g_set_error(error, G_FILE_ERROR, error_code,
            _("Error reading from '%s': %s"),
            file_desc, g_strerror(save_errno));
        goto ERR;
    }

    if (line->len == 0) {
        *into = g_string_free(line, TRUE);
        return 0;
    }

    if (!cp_utf8_validate(line->str, file_desc, error)) {
        goto ERR;
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
cp_canonical_path(const char *path, GError **error) {
    char *path_enc;
    char *result_enc;
    char *result = NULL;

    g_assert(error == NULL || *error == NULL);

    path_enc = g_filename_from_utf8(path, (gssize)-1, NULL, NULL, error);
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

    result = g_filename_to_utf8(result_enc, (gssize)-1, NULL, NULL, error);

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

    path_enc = g_filename_from_utf8(path, (gssize)-1, NULL, NULL, error);
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

    g_assert(data != NULL);

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
