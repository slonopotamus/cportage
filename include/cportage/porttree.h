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

#ifndef CPORTAGE_PORTTREE_H
#define CPORTAGE_PORTTREE_H

#include <glib.h>

#include "cportage/settings.h"

G_BEGIN_DECLS

#pragma GCC visibility push(default)

typedef /*@refcounted@*/ struct CPortagePorttree *CPortagePorttree;

/*@newref@*/ CPortagePorttree
cportage_porttree_new(
    CPortageSettings settings
) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT;

/*@newref@*/ CPortagePorttree
cportage_porttree_ref(CPortagePorttree self);

void
cportage_porttree_unref(/*@killref@*/ /*@null@*/ CPortagePorttree self);

#pragma GCC visibility pop

G_END_DECLS

#endif
