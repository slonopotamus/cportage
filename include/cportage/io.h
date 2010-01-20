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
    I/O utility functions.
 */

#ifndef CPORTAGE_IO_H
#define CPORTAGE_IO_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#pragma GCC visibility push(default)

/*
    Reads file line-by-line an invokes passed function for each line.
    Skips lines that contain \0 char.
    @return true if file was read successfully, false otherwise.
*/
bool cportage_processrawlines(const char * filename,
                     /* Arbitrary object that is passed to func. Useful to accumulate result. */
                     void * ctx,
                     /* Line handling function */
                     void (* func) (void *ctx, char * s));

/*
    Similar to getlines but additionally trims lines, skips empty lines
    and comments starting with #.
 */
bool cportage_processlines(const char * filename,
                  void * ctx,
                  void (* func) (void *ctx, char * s));

#pragma GCC visibility pop

#ifdef __cplusplus
}
#endif

#endif

