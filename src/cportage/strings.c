/*
    Copyright 2009-2014, Marat Radchenko

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

#include "strings.h"

/* This could work much faster with handcoded loop, but i'm lazy */
char **
cp_strings_pysplit(const char *str) {
    /*@only@*/ static GRegex *regex;

    char *trimmed;
    char **result;

    if (regex == NULL) {
        GError *error = NULL;
        regex = g_regex_new("\\s+", 0, 0, &error);
        g_assert_no_error(error);
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
cp_strings_sort(char **str_array) {
    g_qsort_with_data(
        str_array,
        (int)g_strv_length(str_array),
        sizeof(*str_array),
        (GCompareDataFunc)cmpstrp,
        NULL
    );
}

/*@observer@*/ /*@unchecked@*/ static const char * const trues[] = {
    "true", "t", "yes", "y", "1", "on"
};
/*@observer@*/ /*@unchecked@*/ static const char * const falses[] = {
    "false", "f", "no", "n", "0", "off"
};

CPTriBoolean
cp_string_truth(const char *str) {
    size_t i;

    if (str == NULL) {
        return CP_UNKNOWN;
    }

    for (i = 0; i < G_N_ELEMENTS(trues); ++i) {
        if (g_ascii_strcasecmp(trues[i], str) == 0) {
            return CP_TRUE;
        }
    }

    for (i = 0; i < G_N_ELEMENTS(falses); ++i) {
        if (g_ascii_strcasecmp(falses[i], str) == 0) {
            return CP_FALSE;
        }
    }

    return CP_UNKNOWN;
}
