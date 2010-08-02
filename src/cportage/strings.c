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

#include <stdlib.h>

#include "cportage/strings.h"

/* This could work much faster with handcoded loop, but i'm lazy */
char **
cportage_strings_pysplit(const char *str) {
    /*@only@*/ static GRegex *regex;

    char *trimmed;
    char **result;

    if (regex == NULL) {
        regex = g_regex_new("\\s+", 0, 0, NULL);
        g_assert(regex != NULL);
    }

    /* strdup/trim can be avoided if regex will find tokens, not separators */
    trimmed = g_strstrip(g_strdup(str));
    result = g_regex_split(regex, trimmed, 0);

    g_free(trimmed);
    return result;
}

static int
cmpstrp(const void *p1, const void *p2) /*@*/ {
    return g_utf8_collate(*(char * const *)p1, *(char * const *)p2);
}

void
cportage_strings_sort(char **str_array) {
    const size_t len = g_strv_length(str_array);
    qsort(str_array, len, sizeof(*str_array), cmpstrp);
}
