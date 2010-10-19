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

#include <string.h>
#include <sys/utsname.h>

#include "cportage/atom.h"
#include "cportage/io.h"
#include "cportage/settings.h"
#include "cportage/strings.h"

#include "config.h"
#include "actions.h"

static char * G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT
relative_path(const char *base, const char *path)
    /*@modifies errno@*/
{
    /*
      TODO: try using g_file_get_relative_path instead of all this stuff.
      Otherwise, check if it works correctly (preferrably with a testcase) for
      base="/foo" path="/foobar"
     */
    char *path_abs = NULL;
    char *base_abs = NULL;
    char *result;

    g_assert(g_utf8_validate(base, -1, NULL));
    g_assert(g_utf8_validate(path, -1, NULL));

    if ((path_abs = cp_canonical_path(path, NULL)) == NULL) {
        result = g_strdup(path);
    } else if ((base_abs = cp_canonical_path(base, NULL)) == NULL) {
        result = g_strdup(path_abs);
    } else if (g_strstr_len(path_abs, -1, base_abs) == path_abs) {
        result = g_strdup(&path_abs[strlen(base_abs) + 1]);
    } else {
        result = g_strdup(path_abs);
    }

    g_free(path_abs);
    g_free(base_abs);
    return result;
}

static void
print_version(const CPSettings settings, const struct utsname *utsname)
    /*@globals stdout@*/
    /*@modifies fileSystem,errno,*stdout@*/
{
    const char *portdir = cp_settings_get_portdir(settings);
    char *profiles_dir = g_build_filename(portdir, "profiles", NULL);
    const char *profile = cp_settings_get_profile_abs_path(settings);
    char *profile_str = relative_path(profiles_dir, profile);
    /* TODO: read gcc version from gcc-config */
    const char *gcc_ver = "gcc-4.3.2";
    /* TODO: read libc from vartree */
    const char *libc_ver = "glibc-2.8_p20080602-r1";

    g_print("cportage %s (%s, %s, %s, %s %s)\n",
           CP_VERSION, profile_str, gcc_ver, libc_ver,
           utsname->release, utsname->machine);
    g_free(profiles_dir);
    g_free(profile_str);
}

static void
print_porttree_timestamp(const CPSettings settings)
    /*@globals stdout@*/
    /*@modifies fileSystem,errno,*stdout@*/
{
    const char *portdir = cp_settings_get_portdir(settings);
    char *path = g_build_filename(portdir, "metadata", "timestamp.chk", NULL);
    GError *error = NULL;
    char **data = cp_read_lines(path, FALSE, &error);
    g_print("Timestamp of tree: %s\n",
        data != NULL && data[0] != NULL ? data[0] : "Unknown");
    if (error != NULL) {
        g_debug("%s", error->message);
    }
    g_clear_error(&error);
    g_free(path);
    g_strfreev(data);
}

static void
print_packages(const CPSettings settings)
    /*@globals stdout@*/
    /*@modifies fileSystem,errno,*stdout@*/
{
    char *path = g_build_filename(
        cp_settings_get_portdir(settings),
        "profiles", "info_pkgs", NULL
    );
    char **data = cp_read_lines(path, TRUE, NULL);

    if (data != NULL) {
        cp_strings_sort(data);

        CP_STRV_ITER(data, s) {
            CPAtom atom = cp_atom_new(s, NULL);
            char *atom_label = g_strconcat(s, ":", NULL);
            if (atom == NULL) {
                g_print("%-20s [NOT VALID]\n", atom_label);
            } else {
                /* TODO: read version from vdb */
                g_print("%-20s %s\n", atom_label, "3.2_p39");
            }
            g_free(atom_label);
            cp_atom_unref(atom);
        } end_CP_STRV_ITER
    }
    g_free(path);
    g_strfreev(data);
}

static void
print_settings(const CPSettings settings)
    /*@globals stdout@*/
    /*@modifies fileSystem,errno,*stdout@*/
{
    char *path = g_build_filename(
        cp_settings_get_portdir(settings),
        "profiles", "info_vars", NULL
    );
    char **data = cp_read_lines(path, TRUE, NULL);

    if (data != NULL) {
        GString *unset = NULL;
        cp_strings_sort(data);

        CP_STRV_ITER(data, s) {
            const char *value = cp_settings_get(settings, s);
            if (value == NULL) {
                if (unset == NULL) {
                    unset = g_string_append(g_string_new("Unset: "), s);
                } else {
                    g_string_append_printf(unset, ", %s", s);
                }
            } else {
                g_print("%s=\"%s\"\n", s, value);
            }
        } end_CP_STRV_ITER

        if (unset != NULL) {
            g_print("%s\n", unset->str);
        }
        (void)g_string_free(unset, TRUE);
    }
    g_free(path);
    g_strfreev(data);
}

void
cmerge_info_action(const GlobalOptions options, GError **error) {
    CPSettings settings;
    struct utsname utsname;
    /* TODO: read cpu name from /proc/cpuinfo */
    const char *cpu = "ARMv6-compatible_processor_rev_2_-v6l";
    /* TODO: read system name from /etc/gentoo-release */
    const char *sys_version = "gentoo-1.12.11.1";

    g_assert(error == NULL || *error == NULL);

    settings = cp_settings_new(options->config_root, error);
    if (settings == NULL) {
        goto ERR;
    }

    {
        int rc = uname(&utsname);
        g_assert(rc == 0);
    }

    print_version(settings, &utsname);
    g_print("===============================================================\n");
    g_print("System uname: ");
    g_print("%s-%s-%s-%s-with-%s\n",
           utsname.sysname, utsname.release, utsname.machine, cpu, sys_version);
    print_porttree_timestamp(settings);
    print_packages(settings);
    print_settings(settings);

ERR:
    cp_settings_unref(settings);
}
