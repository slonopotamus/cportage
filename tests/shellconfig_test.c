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

#include "cportage/io.h"

static char *dir;

static GHashTable *
assert_parse(const char *path) {
    GHashTable *entries = g_hash_table_new_full(
        g_str_hash, g_str_equal, g_free, g_free
    );
    GError *error = NULL;
    char *full_path = g_build_filename(dir, path, NULL);

    g_assert(cp_read_shellconfig(entries, full_path, true, &error));
    g_assert_no_error(error);

    g_free(full_path);
    return entries;
}

static void
line_cont(void) {
    GHashTable *entries = assert_parse("shellconfig_test_line_cont.conf");
    g_assert(g_hash_table_size(entries) == 1);
    g_assert(g_strcmp0(g_hash_table_lookup(entries, "A"), "BCD") == 0);
    g_hash_table_destroy(entries);
}

static void
simple(void) {
    GHashTable *entries = assert_parse("shellconfig_test_simple.conf");
    g_assert(g_hash_table_size(entries) == 2);
    g_assert(g_strcmp0(g_hash_table_lookup(entries, "A"), "B") == 0);
    g_assert(g_strcmp0(g_hash_table_lookup(entries, "C"), "B") == 0);
    g_hash_table_destroy(entries);
}

static void
source(void) {
    GHashTable *entries = assert_parse("shellconfig_test_source.conf");
    g_assert(g_hash_table_size(entries) == 3);
    g_assert(g_strcmp0(g_hash_table_lookup(entries, "VAR"), "VAL") == 0);
    g_assert(g_strcmp0(g_hash_table_lookup(entries, "A"), "B") == 0);
    g_assert(g_strcmp0(g_hash_table_lookup(entries, "C"), "B") == 0);
    g_hash_table_destroy(entries);
}

int
main(int argc, char *argv[]) {
    g_test_init(&argc, &argv, NULL);

    g_assert(argc == 2);
    dir = argv[1];

    g_test_add_func("/io/shellconfig/simple", simple);
    g_test_add_func("/io/shellconfig/source", source);
    g_test_add_func("/io/shellconfig/line_cont", line_cont);

    return g_test_run();
}
