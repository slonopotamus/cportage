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

#include <string.h>

#include <cportage.h>

static char *dir;

static void
test_var(const char *test_dir, const char *var, const char *expected_value) {
    char *root = g_build_filename(dir, test_dir, NULL);
    GError *error = NULL;
    CPSettings settings;

    GTree *defaults = g_tree_new_full(
        (GCompareDataFunc)strcmp,
        NULL,
        g_free,
        g_free
    );
    g_tree_insert(defaults, g_strdup("PORTDIR"), g_strdup("/tmp"));

    settings = cp_settings_new(root, defaults, &error);

    g_tree_unref(defaults);

    g_assert_no_error(error);
    g_assert_cmpstr(cp_settings_get(settings, var), ==, expected_value);

    cp_settings_unref(settings);
    g_free(root);
}

static void
no_profile(void) {
    test_var("roots/empty", "PORTDIR", "/tmp");
}

static void
profiles_order(void) {
    test_var("roots/profiles_order", "foo", "bar baz");
}

static void
incrementals(void) {
    test_var("roots/incrementals", "USE", "use1 use3 use5");
}

static void
use_mask(void) {
    test_var("roots/use_mask", "USE", "normal unmasked");
}

static void
use_expand(void) {
    test_var("roots/use_expand", "USE", "foo_bar");
    test_var("roots/use_expand_stack", "USE", "foo_bar foo_baz");
    test_var("roots/use_expand_override", "USE", "foo1_baz foo2_bar");
}

static void
use_force(void) {
    test_var("roots/use_force", "USE", "expand_forced normalforced parentforced");

    /* Tests regeneration of USE_EXPAND variables from USE */
    test_var("roots/use_force", "EXPAND", "forced");
}

int
main(int argc, char *argv[]) {
    g_test_init(&argc, &argv, NULL);

    g_assert(argc == 2);
    dir = argv[1];

    g_test_add_func("/settings/no_profile", no_profile);
    g_test_add_func("/settings/profile_order", profiles_order);
    g_test_add_func("/settings/incrementals", incrementals);
    g_test_add_func("/settings/use/mask", use_mask);
    g_test_add_func("/settings/use/expand", use_expand);
    g_test_add_func("/settings/use/force", use_force);

    return g_test_run();
}
