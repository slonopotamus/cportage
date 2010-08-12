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

#include <unistd.h>

#include "cportage/io.h"

static void
assert_valid(const char *path) {
    GHashTable *entries = g_hash_table_new_full(
        g_str_hash, g_str_equal, g_free, g_free
    );
    GError *error = NULL;
    bool success;

    g_assert(access(path, R_OK) == 0);
    success = cp_read_shellconfig(entries, path, false, &error);
    g_assert_no_error(error);
    g_assert(success);

    g_hash_table_destroy(entries);
}

static void
valid_test(void) {
    assert_valid("../../tests/shellconfig/valid01.conf");
    assert_valid("../../tests/shellconfig/valid02.conf");
    assert_valid("../../tests/shellconfig/valid03.conf");
}

/*static void
assert_invalid(const char *path) {
    GHashTable *entries = g_hash_table_new_full(
        g_str_hash, g_str_equal, g_free, g_free
    );
    GError *error = NULL;

    g_assert(access(path, R_OK) == 0);
    g_assert(!cp_read_shellconfig(entries, path, false, &error));
    g_assert(error != NULL);

    g_hash_table_destroy(entries);
}

static void
invalid_test(void) {
    assert_invalid("../../tests/shellconfig/invalid01.conf");
    assert_invalid("../../tests/shellconfig/invalid02.conf");
    assert_invalid("../../tests/shellconfig/invalid03.conf");
}*/

int
main(int argc, char *argv[]) {
    g_test_init(&argc, &argv, NULL);

    g_test_add_func("/io/shellconfig/valid", valid_test);
    /*g_test_add_func("/io/shellconfig/invalid", invalid_test);*/

    return g_test_run();
}
