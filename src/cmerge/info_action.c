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

#include "config.h"

#include <gio/gio.h>
#include <stdlib.h>

#if HAVE_UTSNAME_H
#   include <sys/utsname.h>
#endif

#include <cportage/atom.h>
#include <cportage/io.h>
#include <cportage/strings.h>

#include "actions.h"

static char * G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT
build_profile_str(
    const CPSettings settings,
    const char *portdir
) /*@modifies errno@*/ {
    char *profiles_dir = g_build_filename(portdir, "profiles", NULL);
    const char *profile = cp_settings_profile(settings);
    GFile *profiles_dir_file = g_file_new_for_path(profiles_dir);
    GFile *profile_file = g_file_new_for_path(profile);

    char *result = g_file_get_relative_path(profiles_dir_file, profile_file);
    if (result == NULL) {
        result = g_strconcat("!", profile, NULL);
    }

    g_free(profiles_dir);
    g_object_unref(profiles_dir_file);
    g_object_unref(profile_file);
    return result;
}

static void
print_version(
    const CPSettings settings,
    const struct utsname *utsname,
    const char *portdir
) /*@modifies *stdout,errno@*/ {
    char *profile_str = build_profile_str(settings, portdir);
    /* TODO: read gcc version from gcc-config */
    const char *gcc_ver = "gcc-4.3.2";
    /* TODO: read libc from vartree */
    const char *libc_ver = "glibc-2.8_p20080602-r1";

    g_print("cportage %s (%s, %s, %s, %s %s)\n",
           CP_VERSION, profile_str, gcc_ver, libc_ver,
           utsname->release, utsname->machine);
    g_free(profile_str);
}

static void
print_porttree_timestamp(
    const char *portdir
) /*@modifies errno,*stdout@*/ /*@globals stdout,fileSystem@*/ {
    char *path = g_build_filename(portdir, "metadata", "timestamp.chk", NULL);
    GError *error = NULL;
    FILE *f;
    char *timestamp = NULL;

    /*@-modfilesys@*/
    f = cp_io_fopen(path, "r", &error);
    /*@=modfilesys@*/
    if (f == NULL || cp_io_getline(f, path, &timestamp, &error) < 1) {
        timestamp = g_strdup("Unknown");
    }
    g_assert(timestamp != NULL);
    if (f != NULL) {
        /*@-modfilesys@*/
        (void)fclose(f);
        /*@=modfilesys@*/
    }
    g_free(path);

    g_print("Timestamp of tree: %s\n", g_strstrip(timestamp));
    g_free(timestamp);
    if (error != NULL) {
        g_debug("%s", error->message);
        g_error_free(error);
    }
}

static gboolean G_GNUC_WARN_UNUSED_RESULT
print_atom_matches(
    CPVartree vartree,
    const CPAtom atom,
    char *atom_label,
    GError **error
) /*@modifies vartree,*error,*stdout,errno@*/ {
    GList *match;
    int i = 0;

    if (!cp_vartree_find_packages(vartree, atom, &match, error)) {
        /* Error happened */
        return FALSE;
    }

    if (match == NULL) {
        /* No package matching atom is installed */
        return TRUE;
    }

    g_print("%-20s ", atom_label);
    CP_GLIST_ITER(match, pkg) {
        g_print("%s%s", i++ == 0 ? "" : ", ", cp_package_version(pkg));
    } end_CP_GLIST_ITER
    g_print("\n");

    cp_package_list_free(match);

    return TRUE;
}

static gboolean G_GNUC_WARN_UNUSED_RESULT
print_packages(
    const char *portdir,
    CPVartree vartree,
    GError **error
) /*@modifies vatree,*error,*stdout,errno@*/ /*@globals fileSystem@*/ {
    char *path;
    char **data;
    gboolean result = TRUE;

    g_assert(error == NULL || *error == NULL);

    path = g_build_filename(portdir, "profiles", "info_pkgs", NULL);
    data = cp_io_getlines(path, TRUE, NULL);
    if (data == NULL) {
        goto OUT;
    }

    cp_strings_sort(data);

    CP_STRV_ITER(data, s) {
        CPAtom atom = cp_atom_new(s, NULL);
        char *atom_label = g_strconcat(s, ":", NULL);

        if (atom == NULL) {
            g_print("%-20s [NOT VALID]\n", atom_label);
        } else {
            result = print_atom_matches(vartree, atom, atom_label, error);
        }

        g_free(atom_label);
        cp_atom_unref(atom);
        if (!result) {
            break;
        }
    } end_CP_STRV_ITER

OUT:
    g_free(path);
    g_strfreev(data);

    return result;
}

static void
print_repositories(const CPSettings settings) /*@modifies *stdout,errno@*/ {
    g_print("Repositories:");
    CP_REPOSITORY_ITER(cp_settings_repositories(settings), repo) {
        g_print(" %s", cp_repository_name(repo));
    } end_CP_REPOSITORY_ITER
    g_print("\n");
}

static void
print_settings(
    const CPSettings settings,
    const char *portdir
) /*@modifies *stdout,errno@*/ /*@globals fileSystem@*/ {
    char *path = g_build_filename(portdir, "profiles", "info_vars", NULL);
    char **data = cp_io_getlines(path, TRUE, NULL);

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
            (void)g_string_free(unset, TRUE);
        }
    }
    g_free(path);
    g_strfreev(data);
}

int
cmerge_info_action(
    CPContext ctx,
    /*@unused@*/ const CMergeOptions options G_GNUC_UNUSED,
    GError **error
) {
#if HAVE_UNAME
    struct utsname utsname;
    /* TODO: read cpu name from `uname -p` */
    const char *cpu = "ARMv6-compatible_processor_rev_2_-v6l";
    /* TODO: read system name from /etc/gentoo-release */
    const char *sys_version = "gentoo-1.12.11.1";
#endif

    CPRepository main_repo;
    const char *portdir;
    int rc;

    g_assert(error == NULL || *error == NULL);

    /*@-compdef@*/
    /*@-moduncon@*/
    rc = uname(&utsname);
    /*@=moduncon@*/
    /*@=compdef@*/
    g_assert(rc == 0);

    main_repo = cp_settings_main_repository(ctx->settings);
    portdir = cp_repository_path(main_repo);
    cp_repository_unref(main_repo);

    /*@-compdef@*/
    print_version(ctx->settings, &utsname, portdir);
    /*@=compdef@*/

#if HAVE_UNAME
    g_print("===============================================================\n");
    g_print("System uname: ");
    /*@-compdef@*/
    /*@-usedef@*/
    g_print("%s-%s-%s-%s-with-%s\n",
           utsname.sysname, utsname.release, utsname.machine, cpu, sys_version);
    /*@=usedef@*/
    /*@=compdef@*/
#endif

    print_porttree_timestamp(portdir);
    if (!print_packages(portdir, ctx->vartree, error)) {
        return EXIT_FAILURE;
    }
    print_repositories(ctx->settings);
    print_settings(ctx->settings, portdir);

    return EXIT_SUCCESS;
}
