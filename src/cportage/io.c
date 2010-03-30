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
#include <string.h>

#include "cportage/io.h"

char **
cportage_read_lines(const char *path, const bool ignore_comments, GError **error) {
    char *data;
    char **result;

    g_assert(error == NULL || *error == NULL);

    if (g_file_get_contents(path, &data, NULL, error)) {
        char **lines = g_strsplit(data, "\n", -1);
        free(data);
        if (ignore_comments) {
            char *line;
            int i = 0;
            int j = 0;
            result = g_malloc(g_strv_length(lines) + 1);
            while ((line = lines[i++]) != NULL) {
                char *comment = strchr(line, '#');
                if (comment != NULL)
                    *comment = '\0';
                (void)g_strstrip(line);
                if (line[0] != '\0')
                    result[j++] = line;
            }
            result[j] = NULL;
            g_strfreev(lines);
        } else
            result = lines;
    } else
        result = NULL;
    return result;
}

