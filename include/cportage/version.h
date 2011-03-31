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

#ifndef CP_VERSION_H
#define CP_VERSION_H

#include <glib.h>

/*@-exportany@*/

G_BEGIN_DECLS

#pragma GCC visibility push(default)

/**
 * Structure, describing single package version.
 */
typedef /*@refcounted@*/ struct CPVersion *CPVersion;

/**
 * Creates new #CPVersion structure for \a version.
 *
 * \param version a version string
 * \param error   return location for a %GError, or %NULL
 * \return        a #CPVersion structure, free it using cp_version_unref()
 */
/*@newref@*/ /*@null@*/ CPVersion
cp_version_new(
    const char *version,
    /*@null@*/ GError **error
) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT /*@modifies *error@*/;

/**
 * Increases reference count of \a self by 1.
 *
 * \param self a #CPVersion structure
 * \return \a self
 */
/*@newref@*/ CPVersion
cp_version_ref(CPVersion self) G_GNUC_WARN_UNUSED_RESULT /*@modifies *self@*/;

/**
 * Decreases reference count of \a self by 1. When reference count drops
 * to zero, it frees all the memory associated with the structure.
 *
 * \param self a #CPVersion
 */
void
cp_version_unref(/*@killref@*/ /*@null@*/ CPVersion self) /*@modifies self@*/;

/**
 * Comparator for #CPVersion.
 */
int
cp_version_cmp(
    const CPVersion first,
    const CPVersion second
) G_GNUC_WARN_UNUSED_RESULT /*@*/;

/**
 * \return readonly string representation of \a self
 */
/*@observer@*/ const char *
cp_version_str(const CPVersion self) G_GNUC_WARN_UNUSED_RESULT /*@*/;

#pragma GCC visibility pop

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

G_END_DECLS

#endif
