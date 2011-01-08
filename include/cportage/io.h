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

#if !defined(CPORTAGE_H_INSIDE) && !defined(CPORTAGE_COMPILATION)
#error "Only <cportage.h> can be included directly."
#endif

/** I/O utility functions. */

#ifndef CP_IO_H
#define CP_IO_H

#include <glib.h>
#include <stdio.h>

/*@-exportany@*/

G_BEGIN_DECLS

#pragma GCC visibility push(default)

/**
 * GLib-style fopen() wrapper.
 *
 * \param path  UTF8-encoded filename
 * \param error return location for a %GError, or %NULL
 * \param mode  open mode (see fopen() for possible values)
 * \return      a %FILE pointer or %NULL if an error occured
 */
/*@null@*/ FILE *
cp_fopen(
    const char *path,
    const char *mode,
    /*@null@*/ GError **error
) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT;

/**
 * Reads a single line of input (including line separator if it was encountered).
 * Line is checked to be valid UTF8.
 *
 * \param stream      a %FILE pointer
 * \param stream_desc human-readable description of stream (filename, url, etc)
 *                    in UTF-8 encoding
 * \param into        return location for read line, free it using g_free().
 *                    Only modified on successful read.
 * \param error       return location for a %GError, or %NULL
 * \return            negative number on error, 0 if EOF was reached,
 *                    positive number on successful read
 */
int
cp_getline(
    FILE *stream,
    const char *stream_desc,
    /*@out@*/ char **into,
    /*@null@*/ GError **error
) G_GNUC_WARN_UNUSED_RESULT;

/**
 * GLib-style realpath() wrapper.
 *
 * \param path  UTF8-encoded filename
 * \param error return location for a %GError, or %NULL
 * \return      UTF8-encoded path or %NULL if an error occured,
 *              free it using g_free()
 */
/*@null@*/ char *
cp_canonical_path(
    const char *path,
    /*@null@*/ GError **error
) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT
    /*@modifies *error,errno@*/;

/**
 * Same as g_file_get_contents() but expects UTF8-encoded filename.
 *
 * \param path  UTF8-encoded filename.
 * \param data  location to store an allocated string, free it using g_free()
 * \param len   location to store length in bytes of the contents, or %NULL
 * \param error return location for a %GError, or %NULL
 * \return      %TRUE on success, %FALSE if an error occurred
 */
gboolean
cp_read_file(
    const char *path,
    /*@out@*/ char **data,
    /*@out@*/ size_t *len,
    /*@null@*/ GError **error
) G_GNUC_WARN_UNUSED_RESULT /*@modifies *error,errno@*/;

/**
 * Fully reads text file and splits it at line endings.
 * File contents is checked to be valid UTF8. Skips empty lines.
 *
 * \param path            UTF8-encoded filename
 * \param ignore_comments if %TRUE, comments starting with \c '#' will be
 *                        excluded
 * \param error           return location for a %GError, or %NULL
 * \return                a %NULL-terminated string array
 *                        or %NULL if an error occurred,
 *                        free it using g_strfreev()
 */
/*@null@*/ char **
cp_read_lines(
    const char *path,
    const gboolean ignore_comments,
    /*@null@*/ GError **error
) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT /*@modifies *error,errno@*/;

#pragma GCC visibility pop

G_END_DECLS

#endif
