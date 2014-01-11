/*
    Copyright 2009-2014, Marat Radchenko

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

typedef void (*CPShellconfigSaveFunc)(void *map, char *key, char *value);

typedef const char *(*CPShellconfigLookupFunc)(void *map, const char *key);

/**
 * Reads shell-like config file into a %GHashTable.
 *
 * \param path         UTF8-encoded filename
 * \param into         a map to collect config entries into
 * \param allow_source if %TRUE, \c source statements are handled
 * \param error        return location for a %GError, or %NULL
 * \return             %TRUE on success, %FALSE if an error occurred
 */
gboolean
cp_read_shellconfig(
    void *into,
    CPShellconfigLookupFunc lookup_func,
    CPShellconfigSaveFunc save_func,
    const char *path,
    gboolean allow_source,
    /*@null@*/ GError **error
) G_GNUC_WARN_UNUSED_RESULT
/*@modifies *into,*error,errno@*/ /*@globals fileSystem@*/;

#endif
