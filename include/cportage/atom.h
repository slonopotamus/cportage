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

#ifndef CPORTAGE_ATOM_H
#define CPORTAGE_ATOM_H

#include <glib.h>

G_BEGIN_DECLS

#pragma GCC visibility push(default)

typedef /*@abstract@*/ /*@refcounted@*/ struct CPortageAtom *CPortageAtom;

/*@newref@*/ /*@null@*/ CPortageAtom
cportage_atom_new(const char *str, /*@out@*/ GError **error);

/*@newref@*/ CPortageAtom
cportage_atom_ref(/*@returned@*/ CPortageAtom self) /*@modifies self@*/;

void
cportage_atom_unref(/*@killref@*/ CPortageAtom self) /*@modifies self@*/;

#pragma GCC visibility pop

G_END_DECLS

#endif
