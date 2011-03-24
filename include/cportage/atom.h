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

#ifndef CP_ATOM_H
#define CP_ATOM_H

#include <cportage/package.h>

/*@-exportany@*/

G_BEGIN_DECLS

#pragma GCC visibility push(default)

/**
 * TODO: documentation.
 */
typedef /*@refcounted@*/ struct CPAtom *CPAtom;

/**
 * TODO: documentation.
 */
/*@newref@*/ /*@null@*/ CPAtom
cp_atom_new(
    const char *value,
    /*@null@*/ GError **error
) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT
/*@modifies *error@*/;

/**
 * Increases reference count of \a self by 1.
 *
 * \param self a #CPAtom
 * \return \a self
 */
/*@newref@*/ CPAtom
cp_atom_ref(CPAtom self) G_GNUC_WARN_UNUSED_RESULT /*@modifies *self@*/;

/**
 * Decreases reference count of \a self by 1. When reference count drops
 * to zero, it frees all the memory associated with the structure.
 *
 * \param self a #CPAtom
 */
void
cp_atom_unref(/*@killref@*/ /*@null@*/ CPAtom self) /*@modifies self@*/;

/*@observer@*/ const char *
cp_atom_category(const CPAtom self) G_GNUC_WARN_UNUSED_RESULT /*@*/;

/*@observer@*/ const char *
cp_atom_package(const CPAtom self) G_GNUC_WARN_UNUSED_RESULT /*@*/;

gboolean
cp_atom_matches(
    const CPAtom self,
    const CPPackage package
) G_GNUC_WARN_UNUSED_RESULT /*@modifies package@*/;

gboolean
cp_atom_category_validate(
    const char *category,
    /*@null@*/ GError **error
) G_GNUC_WARN_UNUSED_RESULT /*@modifies *error@*/;

gboolean
cp_atom_slot_validate(
    const char *slot,
    /*@null@*/ GError **error
) G_GNUC_WARN_UNUSED_RESULT /*@modifies *error@*/;

gboolean
cp_atom_pv_split(
    const char *pv,
    /*@out@*/ char **name,
    /*@out@*/ CPVersion *version,
    /*@null@*/ GError **error
) G_GNUC_WARN_UNUSED_RESULT /*@modifies *name,*version,*error@*/;

#pragma GCC visibility pop

G_END_DECLS

#endif
