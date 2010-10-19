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

#ifndef CP_SHELLCONFIG_H
#define CP_SHELLCONFIG_H

#include <glib.h>

/*@-exportany@*/

G_BEGIN_DECLS

#pragma GCC visibility push(default)

#define CP_SHELLCONFIG_ERROR cp_shellconfig_error_quark()

GQuark
cp_shellconfig_error_quark(void);

typedef enum {
    CP_SHELLCONFIG_ERROR_SOURCE_DISABLED,
    CP_SHELLCONFIG_ERROR_SYNTAX
} CPShellConfigError;

/**
 * Reads shell-like config file into given #GHashTable.
 *
 * @param path         UTF8-encoded filename
 * @param into         #GHashTable to collect config entries into
 * @param allow_source if %TRUE, 'source' statements are handled
 * @param error        return location for a #GError, or %NULL
 * @return             %TRUE on success, %FALSE if an error occurred
 */
gboolean
cp_read_shellconfig(
    GHashTable *into,
    const char *path,
    gboolean allow_source,
    /*@null@*/ GError **error
) /*@modifies *into,*error,errno@*/;

#pragma GCC visibility pop

G_END_DECLS

#endif
