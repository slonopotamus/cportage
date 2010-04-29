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

G_BEGIN_DECLS

#pragma GCC visibility push(default)

bool
cportage_read_shellconfig(
    const char *path,
    const bool allow_source,
    GHashTable *into,
    /*@null@*/ GError **error
);

/*@null@*/ char *
cportage_canonical_path(
    const char *path,
    /*@null@*/ GError **error
) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT /*@modifies errno@*/;

/*@null@*/ char **
cportage_read_lines(
    const char *path,
    const bool ignore_comments,
    /*@null@*/ GError **error
) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT /*@modifies errno@*/;

#pragma GCC visibility pop

G_END_DECLS

#endif
