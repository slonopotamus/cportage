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
    g_assert(g_hash_table_size(entries) == 12);
    g_assert_cmpstr(g_hash_table_lookup(entries, "A"), ==, "B");
    g_assert_cmpstr(g_hash_table_lookup(entries, "C"), ==, "B");
    g_assert_cmpstr(g_hash_table_lookup(entries, "C1_A1"), ==, "B");
    g_assert_cmpstr(g_hash_table_lookup(entries, "D_2B"), ==, "A=B C=B");
    g_assert_cmpstr(g_hash_table_lookup(entries, "E"), ==, "");
    g_assert_cmpstr(g_hash_table_lookup(entries, "F"), ==, "0 1 2");
    g_assert_cmpstr(g_hash_table_lookup(entries, "G"), ==, "BG");
    g_assert_cmpstr(g_hash_table_lookup(entries, "H"), ==, "$G");
    g_assert_cmpstr(g_hash_table_lookup(entries, "I"), ==, "${G}");
    g_assert_cmpstr(g_hash_table_lookup(entries, "NONREF"), ==, "");
    g_assert_cmpstr(g_hash_table_lookup(entries, "SHELL"), ==, "/bin/bash");
    g_assert_cmpstr(g_hash_table_lookup(entries, "UNICODE"), ==, "юникод");
    g_hash_table_destroy(entries);
}

static void
comments(void) {
    GHashTable *entries = assert_parse("shellconfig_test_comments.conf");
    g_assert(g_hash_table_size(entries) == 2);
    g_assert_cmpstr(g_hash_table_lookup(entries, "A"), ==, "B");
    g_assert_cmpstr(g_hash_table_lookup(entries, "C"), ==, "B");
    g_hash_table_destroy(entries);
}

static void
line_cont(void) {
    GHashTable *entries = assert_parse("shellconfig_test_line_cont.conf");
    g_assert(g_hash_table_size(entries) == 1);
    g_assert_cmpstr(g_hash_table_lookup(entries, "A"), ==, "BCD");
    g_hash_table_destroy(entries);
}

static void
source(void) {
    GHashTable *entries = assert_parse("shellconfig_test_source.conf");
    g_assert(g_hash_table_size(entries) == 2);
    g_assert_cmpstr(g_hash_table_lookup(entries, "VAR1"), ==, "VAL1");
    g_assert_cmpstr(g_hash_table_lookup(entries, "VAR2"), ==, "VAL2");
    g_hash_table_destroy(entries);
}

static void
escapes(void) {
    GHashTable *entries = assert_parse("shellconfig_test_escapes.conf");
    g_assert(g_hash_table_size(entries) == 4);
    g_assert_cmpstr(g_hash_table_lookup(entries, "A"), ==, "$A");
    g_assert_cmpstr(g_hash_table_lookup(entries, "B"), ==, "${B}");
    g_assert_cmpstr(g_hash_table_lookup(entries, "C"), ==, "$C");
    g_assert_cmpstr(g_hash_table_lookup(entries, "D"), ==, "${D}");
    g_hash_table_destroy(entries);
}

static void
exotic(void) {
    GHashTable *entries = assert_parse("shellconfig_test_exotic.conf");
    g_assert(g_hash_table_size(entries) == 4);
    /* g_assert_cmpstr(g_hash_table_lookup(entries, "F"     ), ==, "#");
    g_assert_cmpstr(g_hash_table_lookup(entries, "G"     ), ==, "##"); */
    g_assert_cmpstr(g_hash_table_lookup(entries, "source"), ==, "source");
    g_assert_cmpstr(g_hash_table_lookup(entries, "export"), ==, "export");
    g_assert_cmpstr(g_hash_table_lookup(entries, "E"     ), ==, "a b");
    g_assert_cmpstr(g_hash_table_lookup(entries, "E1"    ), ==, "a b");
    g_hash_table_destroy(entries);
}

static void
expand_simple(void) {
    GHashTable *entries = g_hash_table_new_full(
        g_str_hash, g_str_equal, g_free, g_free
    );
    char *expanded;
    g_hash_table_insert(entries, g_strdup("PID"), g_strdup("123"));

    expanded = cp_varexpand("", entries, NULL);
    g_assert_cmpstr(expanded, ==, "");
    g_free(expanded);

    expanded = cp_varexpand("abc", entries, NULL);
    g_assert_cmpstr(expanded, ==, "abc");
    g_free(expanded);

    expanded = cp_varexpand(" ", entries, NULL);
    g_assert_cmpstr(expanded, ==, " ");
    g_free(expanded);

    expanded = cp_varexpand("${PID}", entries, NULL);
    g_assert_cmpstr(expanded, ==, "123");
    g_free(expanded);

    g_hash_table_destroy(entries);
}

int
main(int argc, char *argv[]) {
    g_test_init(&argc, &argv, NULL);

    g_assert(argc == 2);
    dir = argv[1];

    g_test_add_func("/shellconfig/read/empty", empty);
    g_test_add_func("/shellconfig/read/eol", eol);
    g_test_add_func("/shellconfig/read/simple", simple);
    g_test_add_func("/shellconfig/read/comments", comments);
    g_test_add_func("/shellconfig/read/line_cont", line_cont);
    g_test_add_func("/shellconfig/read/source", source);
    g_test_add_func("/shellconfig/read/escapes", escapes);
    g_test_add_func("/shellconfig/read/exotic", exotic);

    g_test_add_func("/shellconfig/expand/simple", expand_simple);

    return g_test_run();
}
