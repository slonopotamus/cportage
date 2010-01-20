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

#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "cportage/strings.h"

char * cportage_concat(const char * first, const char * second) {
    assert(first);
    assert(second);
    const size_t firstlen = strlen(first);
    const size_t secondlen = strlen(second);
    char * result = malloc(firstlen + secondlen + 1);
    assert(result);
    memcpy(result, first, firstlen);
    memcpy(result + firstlen, second, secondlen);
    result[firstlen + secondlen] = '\0';
    return result;
}

void cportage_trim(char * s) {
    assert(s);
    size_t first = 0;
    while (s[first] && isspace(s[first]))
        ++first;
    size_t last = strlen(s) - 1;
    while (last > first && isspace(s[last]))
        --last;
    if (first != 0)
        memmove(s, s + first, last - first + 1);
    s[last - first + 1] = '\0';
}
