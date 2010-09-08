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

#include "cportage/shellconfig.h"

static char *dir;

static GHashTable *
assert_parse(const char *path) {
    GHashTable *entries = g_hash_table_new_full(
        g_str_hash, g_str_equal, g_free, g_free
    );
    GError *error = NULL;
    char *full_path = g_build_filename(dir, path, NULL);
    gboolean retval = cp_read_shellconfig(entries, full_path, TRUE, &error);

    g_assert_no_error(error);
    g_assert(retval);

    g_free(full_path);
    return entries;
}

static void
empty(void) {
    GHashTable *entries = assert_parse("shellconfig_test_empty.conf");
    g_assert(g_hash_table_size(entries) == 0);
    g_hash_table_destroy(entries);
}

static void
eol(void) {
    GHashTable *entries = assert_parse("shellconfig_test_eol.conf");
    g_assert(g_hash_table_size(entries) == 0);
    g_hash_table_destroy(entries);
}

static void
simple(void) {
    GHashTable *entries = assert_parse("shellconfig_test_simple.conf");
    g_assert(g_hash_table_size(entries) == 9);
    g_assert(g_strcmp0(g_hash_table_lookup(entries, "A"), "B") == 0);
    g_assert(g_strcmp0(g_hash_table_lookup(entries, "C"), "B") == 0);
    g_assert(g_strcmp0(g_hash_table_lookup(entries, "C1_A1"), "B") == 0);
    g_assert(g_strcmp0(g_hash_table_lookup(entries, "D_2B"), "A=B C=B") == 0);
    g_assert(g_strcmp0(g_hash_table_lookup(entries, "E"), "") == 0);
    g_assert(g_strcmp0(g_hash_table_lookup(entries, "F"), "0 1 2") == 0);
    g_assert(g_strcmp0(g_hash_table_lookup(entries, "G"), "BG") == 0);
    g_assert(g_strcmp0(g_hash_table_lookup(entries, "NONREF"), "") == 0);
    g_assert(g_strcmp0(g_hash_table_lookup(entries, "SHELL"), "/bin/bash") == 0);
    g_hash_table_destroy(entries);
}

static void
comments(void) {
    GHashTable *entries = assert_parse("shellconfig_test_comments.conf");
    g_assert(g_hash_table_size(entries) == 2);
    g_assert(g_strcmp0(g_hash_table_lookup(entries, "A"), "B") == 0);
    g_assert(g_strcmp0(g_hash_table_lookup(entries, "C"), "B") == 0);
    g_hash_table_destroy(entries);
}

static void
line_cont(void) {
    GHashTable *entries = assert_parse("shellconfig_test_line_cont.conf");
    g_assert(g_hash_table_size(entries) == 1);
    g_assert(g_strcmp0(g_hash_table_lookup(entries, "A"), "BCD") == 0);
    g_hash_table_destroy(entries);
}

static void
source(void) {
    GHashTable *entries = assert_parse("shellconfig_test_source.conf");
    g_assert(g_hash_table_size(entries) == 2);
    g_assert(g_strcmp0(g_hash_table_lookup(entries, "VAR1"), "VAL1") == 0);
    g_assert(g_strcmp0(g_hash_table_lookup(entries, "VAR2"), "VAL2") == 0);
    g_hash_table_destroy(entries);
}

static void
escapes(void) {
    GHashTable *entries = assert_parse("shellconfig_test_escapes.conf");
    g_assert(g_hash_table_size(entries) == 4);
    g_assert(g_strcmp0(g_hash_table_lookup(entries, "A"), "$A") == 0);
    g_assert(g_strcmp0(g_hash_table_lookup(entries, "B"), "${B}") == 0);
    g_assert(g_strcmp0(g_hash_table_lookup(entries, "C"), "$C") == 0);
    g_assert(g_strcmp0(g_hash_table_lookup(entries, "D"), "${D}") == 0);
    g_hash_table_destroy(entries);
}

static void
exotic(void) {
    GHashTable *entries = assert_parse("shellconfig_test_exotic.conf");
    g_assert(g_hash_table_size(entries) == 5);
    g_assert(g_strcmp0(g_hash_table_lookup(entries, "F"), "#") == 0);
    g_assert(g_strcmp0(g_hash_table_lookup(entries, "G"), "##") == 0);
    g_assert(g_strcmp0(g_hash_table_lookup(entries, "source"), "source") == 0);
    g_assert(g_strcmp0(g_hash_table_lookup(entries, "export"), "export") == 0);
    g_assert(g_strcmp0(g_hash_table_lookup(entries, "E"), "a b") == 0);
    g_hash_table_destroy(entries);
}

int
main(int argc, char *argv[]) {
    g_test_init(&argc, &argv, NULL);

    g_assert(argc == 2);
    dir = argv[1];

    g_test_add_func("/io/shellconfig/empty", empty);
    g_test_add_func("/io/shellconfig/eol", eol);
    g_test_add_func("/io/shellconfig/simple", simple);
    g_test_add_func("/io/shellconfig/comments", comments);
    g_test_add_func("/io/shellconfig/line_cont", line_cont);
    g_test_add_func("/io/shellconfig/source", source);
    g_test_add_func("/io/shellconfig/escapes", escapes);
    g_test_add_func("/io/shellconfig/exotic", exotic);

    return g_test_run();
}
