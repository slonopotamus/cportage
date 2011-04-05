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

#include <cportage/strings.h>

static void
strv_check(char **strv, ...) {
    int i = 0;
    va_list list;

    g_assert(strv != NULL);

    va_start(list, strv);
    while (TRUE) {
        const char *str = va_arg(list, const char *);
        if (strv[i] == NULL) {
            g_assert(str == NULL);
            break;
        } else if (str == NULL) {
            break;
        } else {
            g_assert_cmpstr(str, ==, strv[i]);
        }
        i++;
    }
    va_end(list);

    g_strfreev(strv);
}

static void
pysplit_test(void) {
    strv_check(cp_strings_pysplit(""), NULL);
    strv_check(cp_strings_pysplit(" "), NULL);
    strv_check(cp_strings_pysplit("  "), NULL);
    strv_check(cp_strings_pysplit("a"), "a", NULL);
    strv_check(cp_strings_pysplit("ab"), "ab", NULL);
    strv_check(cp_strings_pysplit("a b"), "a", "b", NULL);
    strv_check(cp_strings_pysplit("a "), "a", NULL);
    strv_check(cp_strings_pysplit(" a"), "a", NULL);
    strv_check(cp_strings_pysplit(" a "), "a", NULL);
    strv_check(cp_strings_pysplit("a  b"), "a", "b", NULL);
    strv_check(cp_strings_pysplit(" a\tb\nc\r\nd   e\n"),
        "a", "b", "c", "d", "e", NULL);
}

int
main(int argc, char *argv[]) {
    g_test_init(&argc, &argv, NULL);

    g_test_add_func("/strings/pysplit", pysplit_test);

    return g_test_run();
}
