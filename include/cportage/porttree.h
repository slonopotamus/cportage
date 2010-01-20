/*
    Copyright 2009, Marat Radchenko

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

#pragma GCC visibility push(default)

void * cportage_initCPortagePorttree(void);

/* new(Class(CPortagePorttree), settings) */
extern const void * CPortagePorttree;

/*
    Constructs absolute path from tree root.
    It's up to the caller to free result.
    @relative must have leading slash.
 */
char * cportage_porttree_get_path(const void * self, const char * relative);

#pragma GCC visibility pop

#endif

