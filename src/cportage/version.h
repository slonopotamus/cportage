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
