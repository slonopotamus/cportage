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

#include <cportage.h>

static char *dir;

static void
profiles_order(void) {
    char *root = g_build_filename(dir, "roots/profiles_order", NULL);
    GError *error = NULL;
    CPSettings settings = cp_settings_new(root, &error);

    g_assert_no_error(error);
    g_assert_cmpstr(cp_settings_get(settings, "foo"), ==, "bar baz");

    cp_settings_unref(settings);
    g_free(root);
}

static void
incrementals(void) {
    char *root = g_build_filename(dir, "roots/incrementals", NULL);
    GError *error = NULL;
    CPSettings settings = cp_settings_new(root, &error);
    const char *use;

    g_assert_no_error(error);
    g_assert(settings != NULL);
    use = cp_settings_get(settings, "USE");
    g_assert_cmpstr(use, ==, "use1 -use2 use3 -use4 use5 -use6");

    cp_settings_unref(settings);
    g_free(root);
}

int
main(int argc, char *argv[]) {
    g_test_init(&argc, &argv, NULL);

    g_assert(argc == 2);
    dir = argv[1];

    g_test_add_func("/settings/profile_order", profiles_order);
    g_test_add_func("/settings/incrementals", incrementals);

    return g_test_run();
}
