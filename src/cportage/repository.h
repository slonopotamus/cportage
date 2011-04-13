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

#ifndef CP_REPOSITORY_H
#define CP_REPOSITORY_H

#include <cportage.h>

/*@-exportany@*/

/**
 * Creates a #CPRepository with given path. Assumes path is absolute.
 *
 * \return a #CPRepository, free it using cp_repository_unref()
 */
/*@newref@*/ CPRepository
cp_repository_new(
    const char *path
) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT
/*@modifies *stderr,errno@*/ /*@globals fileSystem@*/;

#endif
