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

#ifndef CP_EAPI_H
#define CP_EAPI_H

#include <glib.h>

#include <cportage.h>

/*@-exportany@*/

/**
 * \param eapi  EAPI string
 * \param file  a pathname that contains \a eapi in the GLib file name encoding
 * \param error return location for a %GError, or %NULL
 * \return      %TRUE if \a eapi is supported EAPI version string,
 *              %FALSE otherwise
 */
CPEapi
cp_eapi_parse(
    const char *eapi,
    const char *file,
    /*@null@*/ GError **error
) G_GNUC_WARN_UNUSED_RESULT /*@modifies *error@*/;

/*@observer@*/ const char *
cp_eapi_str(CPEapi eapi) /*@*/;

/**
 * \param file  a pathname in the GLib file name encoding
 * \param error return location for a %GError, or %NULL
 * \return      %TRUE if \a file contains supported EAPI version string,
 *              %FALSE otherwise
 */
CPEapi
cp_eapi_parse_file(
    const char *file,
    /*@null@*/ GError **error
) G_GNUC_WARN_UNUSED_RESULT /*@modifies *error,errno@*/ /*@globals fileSystem@*/;

gboolean
cp_eapi_check(CPEapi eapi, /*@null@*/ GError **error) /*@modifies *error@*/;

gboolean
cp_eapi_has_use_deps(CPEapi eapi) G_GNUC_WARN_UNUSED_RESULT G_GNUC_PURE /*@*/;

gboolean
cp_eapi_has_slot_deps(CPEapi eapi) G_GNUC_WARN_UNUSED_RESULT G_GNUC_PURE /*@*/;

gboolean
cp_eapi_has_strong_blocks(CPEapi eapi) G_GNUC_WARN_UNUSED_RESULT G_GNUC_PURE /*@*/;

#endif
