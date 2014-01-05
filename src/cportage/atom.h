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

#ifndef CP_ATOM_H
#define CP_ATOM_H

#include <cportage.h>

/*@-exportany@*/

/**
 * \return readonly category name of \a self
 */
/*@observer@*/ const char *
cp_atom_category(const CPAtom self) G_GNUC_WARN_UNUSED_RESULT /*@*/;

/**
 * \return readonly package name of \a self
 */
/*@observer@*/ const char *
cp_atom_package(const CPAtom self) G_GNUC_WARN_UNUSED_RESULT /*@*/;

/**
 * \param error return location for a %GError, or %NULL
 * \return      %TRUE if \a category is a valid category name, %FALSE otherwise
 */
gboolean
cp_atom_category_validate(
    const char *category,
    /*@null@*/ GError **error
) G_GNUC_WARN_UNUSED_RESULT /*@modifies *error@*/;

/**
 * \param error return location for a %GError, or %NULL
 * \return      %TRUE if \a slot is a valid slot name, %FALSE otherwise
 */
gboolean
cp_atom_slot_validate(
    const char *slot,
    CPEapi eapi,
    /*@null@*/ GError **error
) G_GNUC_WARN_UNUSED_RESULT /*@modifies *error@*/;

/**
 * \param error return location for a %GError, or %NULL
 * \return      %TRUE if \a repo is a valid repository name, %FALSE otherwise
 */
gboolean
cp_atom_repo_validate(
    const char *repo,
    /*@null@*/ GError **error
) G_GNUC_WARN_UNUSED_RESULT /*@modifies *error@*/;

/**
 * Splits package-version string \a pv into \a name and \a version.
 *
 * \param name    return location for package name, set to %NULL on error.
 *                Free it using g_free().
 * \param version return location for package version, set to %NULL on error.
 *                Free it using cp_version_unref().
 * \param error   return location for a %GError, or %NULL
 * \return        %TRUE if \a pv is a valid package-version string,
 *                %FALSE otherwise
 */
gboolean
cp_atom_pv_split(
    const char *pv,
    /*@out@*/ char **name,
    /*@out@*/ CPVersion *version,
    /*@null@*/ GError **error
) G_GNUC_WARN_UNUSED_RESULT /*@modifies *name,*version,*error@*/;

#endif
