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

#include "cportage/object.h"

#ifdef __cplusplus
extern "C" {
#endif

#pragma GCC visibility push(default)

    void * cportage_initCPortageAtom(void);

    /* new(Class(Atom), "=foo/bar-1.0") */
    extern const void * CPortageAtom;

#pragma GCC visibility pop

#ifdef __cplusplus
}
#endif

#endif
