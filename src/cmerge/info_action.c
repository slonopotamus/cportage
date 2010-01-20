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

#include <assert.h>
#include <libiberty.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>

#include "cportage/atom.h"
#include "cportage/io.h"
#include "cportage/porttree.h"
#include "cportage/settings.h"
#include "cportage/strings.h"

#include "config.h"
#include "cmerge/actions.h"

static void print_version(const void * settings, const struct utsname * utsname) {
    char * profile = cportage_settings_get_profile(settings);
    char * abs_profile_path = realpath(profile, NULL);
    if (abs_profile_path) {
        char * portdir = cportage_settings_get_portdir(settings);
        char * profiles_dir = concat(portdir, "/profiles");
        char * abs_profiles_dir_path = realpath(profiles_dir, NULL);
        if (abs_profiles_dir_path) {
            if (strstr(abs_profile_path, abs_profiles_dir_path) == abs_profile_path) {
                const size_t profiles_dir_len = strlen(abs_profiles_dir_path);
                const char * src = &abs_profile_path[profiles_dir_len + 1];
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

    // TODO: read gcc version from gcc-config
    const char * gcc_ver = "gcc-4.3.2";
    // TODO: read libc from vartree
    const char * libc_ver = "glibc-2.8_p20080602-r1";

    printf("cportage %s (%s, %s, %s, %s %s)\n",
           CPORTAGE_VERSION, profile, gcc_ver, libc_ver,
           utsname->release, utsname->machine);
    free(profile);
}

static void print_timestamp_line(void *ctx, char * s) {
    bool * need_newline = ctx;
    * need_newline = s[strlen(s) - 1] != '\n';
    fputs(s, stdout);
}

static void print_porttree_timestamp(const void * porttree) {
    char * filename = cportage_porttree_get_path(porttree, "/metadata/timestamp.chk");
    fputs("Timestamp of tree: ", stdout);
    bool need_newline = true;
    if (cportage_processrawlines(filename, &need_newline, &print_timestamp_line)) {
        if (need_newline)
            puts("");
    } else
        puts("Unknown");
    free(filename);
}

static void print_package(void * ctx __attribute__((unused)), char * s) {
    void * atom = cportage_new(CPortageClass(CPortageAtom), s);
    char * s1 = concat(s, ":", NULL);
    if (atom) {
        const char * version = "3.2_p39";
        printf("%-20s %s\n", s1, version);
    } else
        printf("%-20s %s\n", s1, "[NOT VALID]");
    free(s1);
    cportage_unref(atom);
}

static void print_packages(const void * porttree) {
    char * filename = cportage_porttree_get_path(porttree, "/profiles/info_pkgs");
    cportage_processlines(filename, NULL, &print_package);
    free(filename);
}

static void print_setting(void * settings, char * s) {
    char * value = cportage_settings_get(settings, s);
    if (value)
        printf("%s=\"%s\"\n", s, value);
    else {
        printf("%s unset\n", s);
    }
    free(value);
}

static void print_settings(const void * porttree, void * settings) {
    char * filename = cportage_porttree_get_path(porttree, "/profiles/info_vars");
    cportage_processlines(filename, settings, &print_setting);
    free(filename);
}

int cmerge_info_action(const struct cmerge_gopts * options) {
    void * settings = cportage_new(CPortageClass(CPortageSettings), options->config_root);
    if (!settings)
        return EXIT_FAILURE;

    void * porttree = cportage_new(CPortageClass(CPortagePorttree), settings);
    assert(porttree);

    struct utsname utsname;
    int rc = uname(&utsname);
    assert(rc == 0);

    print_version(settings, &utsname);
    puts("=================================================================");
    fputs("System uname: ", stdout);
    // TODO: read cpu name from /proc/cpuinfo
    const char * cpu = "ARMv6-compatible_processor_rev_2_-v6l";
    // TODO: read system name from /etc/gentoo-release
    const char * system = "gentoo-1.12.11.1";
    printf("%s-%s-%s-%s-with-%s\n",
           utsname.sysname, utsname.release, utsname.machine, cpu, system);
    print_porttree_timestamp(porttree);
    print_packages(porttree);
    print_settings(porttree, settings);

    cportage_unref(porttree);
    cportage_unref(settings);
    return EXIT_SUCCESS;
}
