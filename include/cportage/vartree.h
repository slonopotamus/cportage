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

#ifndef CP_VARTREE_H
#define CP_VARTREE_H

#include <cportage/atom.h>
#include <cportage/settings.h>

/*@-exportany@*/

G_BEGIN_DECLS

#pragma GCC visibility push(default)

/**
 * Installed packages tree.
 */
typedef /*@refcounted@*/ struct CPVartree *CPVartree;

/**
 * TODO: documentation.
 */
/*@newref@*/ CPVartree
cp_vartree_new(
    const CPSettings settings
) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT /*@*/;

/**
 * Increases reference count of \a self by 1.
 *
 * \param self a #CPVartree structure
 * \return \a self
 */
/*@newref@*/ CPVartree
cp_vartree_ref(CPVartree self) G_GNUC_WARN_UNUSED_RESULT /*@modifies *self@*/;

/**
 * Decreases reference count of \a self by 1. When reference count drops
 * to zero, it frees all the memory associated with the structure.
 *
 * \param self a #CPVartree
 */
void
cp_vartree_unref(/*@killref@*/ /*@null@*/ CPVartree self) /*@modifies self@*/;

/**
 * TODO: documentation.
 */
gboolean
cp_vartree_find_packages(
    CPVartree self,
    const CPAtom atom,
    /*@out@*/ GList/*<CPPackage>*/ **match,
    /*@null@*/ GError **error
) G_GNUC_WARN_UNUSED_RESULT
/*@modifies self,*match,*error@*/ /*@globals fileSystem@*/;

#pragma GCC visibility pop

G_END_DECLS

#endif
