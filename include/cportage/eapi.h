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

#ifndef CP_EAPI_H
#define CP_EAPI_H

#include <glib.h>

/*@-exportany@*/

G_BEGIN_DECLS

#pragma GCC visibility push(default)

/**
 * \param eapi  EAPI string
 * \param file  a pathname that contains \a eapi in the GLib file name encoding
 * \param error return location for a %GError, or %NULL
 * \return      %TRUE if \a eapi is supported EAPI version string,
 *              %FALSE otherwise
 */
gboolean
cp_eapi_check(
    const char *eapi,
    const char *file,
    /*@null@*/ GError **error
) G_GNUC_WARN_UNUSED_RESULT /*@modifies *error@*/;

/**
 * \param file  a pathname in the GLib file name encoding
 * \param error return location for a %GError, or %NULL
 * \return      %TRUE if \a file contains supported EAPI version string,
 *              %FALSE otherwise
 */
gboolean
cp_eapi_check_file(
    const char *file,
    /*@null@*/ GError **error
) G_GNUC_WARN_UNUSED_RESULT /*@modifies *error,errno@*/ /*@globals fileSystem@*/;

#pragma GCC visibility pop

G_END_DECLS

#endif
