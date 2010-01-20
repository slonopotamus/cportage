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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "cportage/strings.h"

struct item {
    const char * first;
    const char * second;
};

int main(void) {
    struct item data[] = {
        { "", "" },
        { "a", "" },
        { "", "a" },
        { "a", "b" }
    };
    int retval = 0;
    for (unsigned int i = 0; i < sizeof(data) / sizeof(data[0]); ++i) {
        char * s = cportage_concat(data[i].first, data[i].second);
        const size_t expected_len = strlen(data[i].first)
                                    + strlen(data[i].second);
        if (strlen(s) != expected_len) {
            fprintf(stderr, "concat(%s, %s), expected len == %zd but got %zd\n",
                    data[i].first, data[i].second, expected_len, strlen(s));
            --retval;
        }
        free(s);
    }
    return retval;
}
