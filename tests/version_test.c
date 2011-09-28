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

#include <cportage/version.h>

static int
signum(int i) {
    return (i > 0) - (i < 0);
}

static void
assert_version_cmp(const char *first, const char *second, int expected) {
    GError *error = NULL;
    CPVersion f, s;
    int actual;

    f = cp_version_new(first, &error);
    g_assert_no_error(error);
    g_assert(f != NULL);

    s = cp_version_new(second, &error);
    g_assert_no_error(error);
    g_assert(s != NULL);

    actual = signum(cp_version_cmp(f, s));
    if (actual != expected) {
        g_error("%s cmp %s: expected %d but got %d",
                first, second, expected, actual);
    }

    actual = signum(cp_version_cmp(s, f));
    if (actual != -expected) {
        g_error("%s cmp %s: expected %d but got %d",
                first, second, -expected, actual);
    }

    cp_version_unref(f);
    cp_version_unref(s);
}

static void
version_cmp(void) {
    assert_version_cmp("4.0", "4.0", 0);
    assert_version_cmp("1.0", "1.0", 0);
    assert_version_cmp("1.0-r0", "1.0", 0);
    assert_version_cmp("1.0", "1.0-r0", 0);
    assert_version_cmp("1.0-r0", "1.0-r0", 0);
    assert_version_cmp("1.0-r1", "1.0-r1", 0);

    assert_version_cmp("4.0", "5.0", -1);
    assert_version_cmp("5", "5.0", -1);
    assert_version_cmp("1.0_pre2","1.0_p2", -1);
    assert_version_cmp("1.0_alpha2", "1.0_p2", -1);
    assert_version_cmp("1.0_alpha1", "1.0_beta1", -1);
    assert_version_cmp("1.0_beta3","1.0_rc3", -1);
    assert_version_cmp("1.001000000000000000001", "1.001000000000000000002", -1);
    assert_version_cmp("1.00100000000", "1.0010000000000000001", -1);
    assert_version_cmp("999999999999999999999999999998",
        "999999999999999999999999999999", -1);
    assert_version_cmp("1.01", "1.1", -1);
    assert_version_cmp("1.0-r0", "1.0-r1", -1);
    assert_version_cmp("1.0", "1.0-r1", -1);
    assert_version_cmp("1.0", "1.0.0", -1);
    assert_version_cmp("1.0b", "1.0.0", -1);
    assert_version_cmp("1_p1", "1b_p1", -1);
    assert_version_cmp("1", "1b", -1);
    assert_version_cmp("1.1", "1.1b", -1);
    assert_version_cmp("12.2b", "12.2.5", -1);

    assert_version_cmp("12.2", "12.2a", -1);
    assert_version_cmp("12.2a", "12.2b", -1);

    /* Test that comparison is really numeric */
    assert_version_cmp("2", "11", -1);
    assert_version_cmp("1.2", "1.11", -1);
    assert_version_cmp("1_beta2", "1_beta11", -1);
    assert_version_cmp("1-r2", "1-r11", -1);
}

int
main(int argc, char *argv[]) {
    g_test_init(&argc, &argv, NULL);

    g_test_add_func("/version/cmp", version_cmp);

    return g_test_run();
}
