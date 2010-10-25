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

#ifndef CP_ATOM_H
#define CP_ATOM_H

#include <glib.h>

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
    const char *str,
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
cp_atom_unref(
    /*@killref@*/ /*@null@*/ CPAtom self
) /*@modifies self@*/;

#pragma GCC visibility pop

G_END_DECLS

#endif
