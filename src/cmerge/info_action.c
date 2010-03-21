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
#include <sys/utsname.h>

#include "cportage/atom.h"
#include "cportage/io.h"
#include "cportage/porttree.h"

#include "config.h"
#include "cmerge/actions.h"

static void
print_version(const CPortageSettings settings, const struct utsname *utsname) {
    char *profile = cportage_settings_get_profile(settings);
    char *abs_profile_path = realpath(profile, NULL);
    /* TODO: read gcc version from gcc-config */
    const char *gcc_ver = "gcc-4.3.2";
    /* TODO: read libc from vartree */
    const char *libc_ver = "glibc-2.8_p20080602-r1";

    /* TODO: simplify this crap */
    if (abs_profile_path) {
        char *portdir = cportage_settings_get_portdir(settings);
        char *profiles_dir = g_strconcat(portdir, "/profiles", NULL);
        char *abs_profiles_dir_path = realpath(profiles_dir, NULL);
        if (abs_profiles_dir_path) {
            if (strstr(abs_profile_path, abs_profiles_dir_path) == abs_profile_path) {
                const size_t profiles_dir_len = strlen(abs_profiles_dir_path);
                const char *src = &abs_profile_path[profiles_dir_len + 1];
                const size_t len = strlen(abs_profile_path) - profiles_dir_len;
                memmove(abs_profile_path, src, len);
                free(profile);
                profile = abs_profile_path;
            }
            free(abs_profiles_dir_path);
        }
        if (profile != abs_profile_path) {
          free(abs_profile_path);
        }
        free(profiles_dir);
        free(portdir);
    }

    g_print("cportage %s (%s, %s, %s, %s %s)\n",
           CPORTAGE_VERSION, profile, gcc_ver, libc_ver,
           utsname->release, utsname->machine);
    free(profile);
}

static void
print_porttree_timestamp(const CPortagePorttree porttree) {
    char *path = cportage_porttree_get_path(porttree, "/metadata/timestamp.chk");
    char **data = cportage_read_lines(path, false, NULL);
    g_print("Timestamp of tree: %s\n",
        data != NULL && data[0] != NULL ? data[0] : "Unknown");
    free(path);
    g_strfreev(data);;
}

static void
print_packages(const CPortagePorttree porttree) {
    char *path = cportage_porttree_get_path(porttree, "/profiles/info_pkgs");
    char **data = cportage_read_lines(path, true, NULL);

    if (data != NULL) {
        char *line;
        int i = 0;
        while ((line = data[i++]) != NULL) {
            CPortageAtom atom = cportage_atom_new(line, NULL);
            char *atom_label = g_strconcat(line, ":", NULL);
            if (atom)
                g_print("%-20s %s\n", atom_label, "3.2_p39");
            else
                g_print("%-20s [NOT VALID]\n", atom_label);
            free(atom_label);
            cportage_atom_unref(atom);
        }
    }
    free(path);
    g_strfreev(data);
}

static void
print_settings(const CPortagePorttree porttree, const CPortageSettings settings) {
    char *path = cportage_porttree_get_path(porttree, "/profiles/info_vars");
    char **data = cportage_read_lines(path, true, NULL);

    if (data != NULL) {
        char *line;
        int i = 0;
        while ((line = data[i++]) != NULL) {
            char *value = cportage_settings_get_entry(settings, line, NULL);
            if (value == NULL)
                g_print("%s unset\n", line);
            else
                g_print("%s=\"%s\"\n", line, value);
            free(value);
        }
    }
    free(path);
    g_strfreev(data);
}

void
cmerge_info_action(const GlobalOptions options, GError **error) {
    CPortageSettings settings;
    CPortagePorttree porttree;
    struct utsname utsname;
    int rc;
    /* TODO: read cpu name from /proc/cpuinfo */
    const char *cpu = "ARMv6-compatible_processor_rev_2_-v6l";
    /* TODO: read system name from /etc/gentoo-release */
    const char *system = "gentoo-1.12.11.1";

    g_assert(error == NULL || *error == NULL);

    settings = cportage_settings_new(options->config_root, error);
    if (settings == NULL)
        return;

    porttree = cportage_porttree_new(settings);

    rc = uname(&utsname);
    g_assert(rc == 0);

    print_version(settings, &utsname);
    g_print("=================================================================\n");
    g_print("System uname: ");
    g_print("%s-%s-%s-%s-with-%s\n",
           utsname.sysname, utsname.release, utsname.machine, cpu, system);
    print_porttree_timestamp(porttree);
    print_packages(porttree);
    print_settings(porttree, settings);

    cportage_porttree_unref(porttree);
    cportage_settings_unref(settings);
}
