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

#ifndef CP_VERSION_H
#define CP_VERSION_H

#include <cportage.h>

/*@-exportany@*/

/**
 * Creates new #CPVersion structure for \a version.
 *
 * \param version a valid version string
 * \param error   return location for a %GError, or %NULL
 * \return        a #CPVersion structure, free it using cp_version_unref()
 */
/*@newref@*/ /*@null@*/ CPVersion
cp_version_new(
    const char *version,
    /*@null@*/ GError **error
) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT /*@modifies *error@*/;

/**
 * Performs matching of given versions ignoring revision.
 * Used in '~foo/bar-1' atoms.
 *
 * \return %TRUE if \a first matches \a second, %FALSE otherwise
 */
gboolean
cp_version_any_revision_match(
    const CPVersion first,
    const CPVersion second
) G_GNUC_WARN_UNUSED_RESULT /*@*/;

/**
 * Performs glob matching of given versions.
 * Used in '=foo/bar-1*' atoms.
 *
 * \return %TRUE if \a first matches \a second, %FALSE otherwise
 */
gboolean
cp_version_glob_match(
    const CPVersion first, 
    const CPVersion second
) G_GNUC_WARN_UNUSED_RESULT /*@*/;

#endif
