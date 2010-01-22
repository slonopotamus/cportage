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

/*
    String utility functions.
    I have no idea why this isn't present in standard lib.
 */

#ifndef CPORTAGE_STRINGS_H
#define CPORTAGE_STRINGS_H

#ifdef __cplusplus
extern "C" {
#endif

#pragma GCC visibility push(default)

    /** Trims given string (modifies it inplace). */
    void cportage_trim(char * s);

#pragma GCC visibility pop

#ifdef __cplusplus
}
#endif

#endif
