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

/** Collections functions. */

#ifndef CP_COLLECTIONS_H
#define CP_COLLECTIONS_H

#include <glib.h>

/*@-exportany@*/

void
cp_tree_foreach_remove(
    GTree *tree,
    GHRFunc func,
    /*@null@*/ void *user_data
) /*@modifies *tree,*user_data@*/;

void
cp_hash_table_destroy(
    /*@null@*/ /*@only@*/ GHashTable *hash_table
) /*@modifies hash_table@*/;

void
cp_stack_dict(GTree *tree, char **items) /*@modifies *tree@*/;

#endif
