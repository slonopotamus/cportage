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

/** I/O utility functions. */

#ifndef CPORTAGE_IO_H
#define CPORTAGE_IO_H

#include <stdbool.h>
#include <glib.h>

/*@-exportany@*/

G_BEGIN_DECLS

#pragma GCC visibility push(default)

bool
cportage_read_shellconfig(
    GHashTable *into,
    const char *path,
    bool allow_source,
    /*@null@*/ GError **error
) /*@modifies *into,*error,errno@*/;

/**
 * GLib-style realpath(3) wrapper.
 *
 * @param path UTF8-encoded filename.
 * @param error will be set if error happens.
 * @return UTF8-encoded real path for given filename, NULL on error.
 */
/*@null@*/ char *
cportage_canonical_path(
    const char *path,
    /*@null@*/ GError **error
) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT
    /*@modifies *error,errno@*/;

/**
 * Fully reads text file and splits it at line endings.
 * File is checked to be valid UTF8.
 *
 * @param path UTF8-encoded filename.
 * @param ignore_comments if true, comments starting with '#' will be excluded.
 * @param error will be set if error happens.
 * @return NULL on error.
 */
/*@null@*/ char **
cportage_read_lines(
    const char *path,
    const bool ignore_comments,
    /*@null@*/ GError **error
) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT
    /*@modifies *error,errno@*/;

#pragma GCC visibility pop

G_END_DECLS

#endif
