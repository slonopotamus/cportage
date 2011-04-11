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
 * \param path  a pathname in the GLib file name encoding
 * \param error return location for a %GError, or %NULL
 * \param mode  open mode (see fopen() for possible values)
 * \return      a %FILE pointer or %NULL if an error occured
 */
/*@dependent@*/ /*@null@*/ FILE *
cp_io_fopen(
    const char *path,
    const char *mode,
    /*@null@*/ GError **error
) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT
/*@modifies *error,errno,fileSystem@*/ /*@globals fileSystem@*/;

/**
 * Reads a single line of input (including line separator if it was encountered).
 *
 * \param file      a %FILE pointer
 * \param file_desc human-readable description of stream (filename, url, etc)
 * \param into      return location for read line, free it using g_free().
 *                  Only modified on successful read.
 * \param error     return location for a %GError, or %NULL
 * \return          negative number on error, 0 if EOF was reached,
 *                  positive number on successful read
 */
int
cp_io_getline(
    FILE *file,
    const char *file_desc,
    /*@out@*/ char **into,
    /*@null@*/ GError **error
) G_GNUC_WARN_UNUSED_RESULT
/*@modifies *file,*into,*error,errno@*/ /*@globals fileSystem@*/;

/*@-globuse@*/
/**
 * GLib-style realpath() wrapper.
 *
 * \param path  a pathname in the GLib file name encoding
 * \param error return location for a %GError, or %NULL
 * \return      a pathname in the GLib file name encoding
 *              or %NULL if an error occured, free it using g_free()
 */
/*@null@*/ char *
cp_io_realpath(
    const char *path,
    /*@null@*/ GError **error
) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT
/*@modifies *error,errno@*/ /*@globals errno,fileSystem@*/;
/*@=globuse@*/

/**
 * Fully reads text file and splits it at line endings. Skips empty lines.
 *
 * \param path            a pathname in the GLib file name encoding
 * \param ignore_comments if %TRUE, comments starting with \c '#' will be
 *                        excluded
 * \param error           return location for a %GError, or %NULL
 * \return                a %NULL-terminated string array
 *                        or %NULL if an error occurred,
 *                        free it using g_strfreev()
 */
/*@null@*/ /*@only@*/ char **
cp_io_getlines(
    const char *path,
    const gboolean ignore_comments,
    /*@null@*/ GError **error
) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT
/*@modifies *error,errno@*/ /*@globals fileSystem@*/;

/*@null@*/ /*@only@*/ char *
cp_io_get_relative_path(
    const char *parent,
    const char *descendant
) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT
/*@*/;

#pragma GCC visibility pop

G_END_DECLS

#endif
