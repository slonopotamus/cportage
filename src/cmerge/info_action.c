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

#include "config.h"
#include "actions.h"

#include <stdlib.h>
#include <string.h>

#include <sys/utsname.h>

#include <cportage.h>

static char * G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT
build_profile_str(
    const CPSettings settings,
    const char *portdir
) /*@@*/ {
    char *profiles_dir = g_build_filename(portdir, "profiles", NULL);
    const char *profile = cp_settings_profile(settings);

    char *result = cp_path_get_relative(profiles_dir, profile);
    if (result == NULL) {
        result = g_strconcat("!", profile, NULL);
    }

    g_free(profiles_dir);
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
    CPTree vartree,
    const CPAtom atom,
    char *atom_label,
    GError **error
) /*@modifies vartree,*error,*stdout,errno@*/ {
    GSList *match;
    int i = 0;

    if (!cp_tree_find_packages(vartree, atom, TRUE, &match, error)) {
        /* Error happened */
        return FALSE;
    }

    if (match == NULL) {
        /* No package matching atom is installed */
        return TRUE;
    }

    g_print("%-20s ", atom_label);
    CP_GSLIST_ITER(match, pkg) {
        CPVersion version = cp_package_version(pkg);
        g_print("%s%s", i++ == 0 ? "" : ", ", cp_version_str(version));
        cp_version_unref(version);
    } end_CP_GSLIST_ITER
    g_print("\n");

    cp_package_list_free(match);

    return TRUE;
}

static gboolean G_GNUC_WARN_UNUSED_RESULT
print_packages(
    const char *portdir,
    CPContext ctx,
    /*@null@*/ GError **error
) /*@modifies *ctx,*error,*stdout,errno@*/ /*@globals fileSystem@*/ {
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
        CPAtom atom = cp_atom_new(ctx->atom_factory, CP_EAPI_LATEST, s, NULL);
        char *atom_label = g_strconcat(s, ":", NULL);

        if (atom == NULL) {
            g_print("%-20s [NOT VALID]\n", atom_label);
        } else {
            result = print_atom_matches(ctx->vardb, atom, atom_label, error);
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
    CP_GSLIST_ITER(cp_settings_repositories(settings), repo) {
        g_print(" %s", cp_repository_name(repo));
    } end_CP_GSLIST_ITER
    g_print("\n");
}

static void
print_use(const CPSettings settings) {
    char **use_expand_keys = cp_strings_pysplit(
        cp_settings_get_default(settings, "USE_EXPAND", "")
    );

    g_print("USE=\"%s\"",
        cp_settings_get_default(settings, "CPORTAGE_USE_NO_EXPAND", "")
    );

    CP_STRV_ITER(use_expand_keys, key) {
        const char *value = cp_settings_get_default(settings, key, "");
        if (value[0] != '\0') {
            g_print(" %s=\"%s\"", key, value);
        }
    } end_CP_STRV_ITER

    g_print("\n");

    g_strfreev(use_expand_keys);
}

static void
print_settings(
    const CPSettings settings,
    const char *portdir
) /*@modifies *stdout,errno@*/ /*@globals fileSystem@*/ {
    char *path = g_build_filename(portdir, "profiles", "info_vars", NULL);
    char **data = cp_io_getlines(path, TRUE, NULL);
    GString *unset = NULL;

    if (data == NULL) {
        goto OUT;
    }

    cp_strings_sort(data);

    CP_STRV_ITER(data, s) {
        const char *value = cp_settings_get(settings, s);
        if (value == NULL) {
            if (unset == NULL) {
                unset = g_string_append(g_string_new("Unset:  "), s);
            } else {
                g_string_append_printf(unset, ", %s", s);
            }
        } else if (strcmp(s, "USE") == 0) {
            print_use(settings);
        } else {
            g_print("%s=\"%s\"\n", s, value);
        }
    } end_CP_STRV_ITER

    if (unset != NULL) {
        g_print("%s\n", unset->str);
        (void)g_string_free(unset, TRUE);
    }

OUT:
    g_free(path);
    g_strfreev(data);
}

static char * G_GNUC_WARN_UNUSED_RESULT
get_baselayout_version(CPContext ctx, /*@null@*/ GError **error) {
    CPAtom atom = cp_atom_new(
        ctx->atom_factory, CP_EAPI_LATEST, "sys-apps/baselayout", NULL
    );
    GSList *match = NULL;
    CPVersion version = NULL;
    char *result = NULL;

    g_assert(atom != NULL);

    if (!cp_tree_find_packages(ctx->vardb, atom, FALSE, &match, error)) {
        goto OUT;
    }

    if (match == NULL) {
        result = g_strdup("unknown");
        goto OUT;
    }

    version = cp_package_version(match->data);
    result = g_strdup(cp_version_str(version));

OUT:
    cp_atom_unref(atom);
    cp_package_list_free(match);
    cp_version_unref(version);

    return result;
}

static char * G_GNUC_WARN_UNUSED_RESULT
get_command_output(const char *cmd) {
    int status;
    char *output = NULL;
    char *end;

    if (!g_spawn_command_line_sync(cmd, &output, NULL, &status, NULL)) {
        goto ERR;
    }

    if (output == NULL) {
        goto ERR;
    }

    if (!WIFEXITED(status) || WEXITSTATUS(status) != EXIT_SUCCESS) {
        goto ERR;
    }

    end = strchr(output, '\n');
    if (end != NULL) {
        *end = '\0';
    }

    return output;

ERR:
    g_free(output);
    return NULL;
}

static gboolean G_GNUC_WARN_UNUSED_RESULT
print_system_name(
    CPContext ctx,
    struct utsname *utsname,
    /*@null@*/ GError **error
) {
    char *cpu = NULL;
    char *sys_version = NULL;
    gboolean result = FALSE;

    g_assert(error == NULL || *error == NULL);

    sys_version = get_baselayout_version(ctx, error);
    if (sys_version == NULL) {
        goto ERR;
    }

    cpu = get_command_output("uname -p");

    g_print("=============================================================\n");
    g_print("System uname: ");
    /*@-compdef@*/
    /*@-usedef@*/
    g_print("%s-%s-%s-%s-with-gentoo-%s\n",
        utsname->sysname,
        utsname->release,
        utsname->machine,
        cpu == NULL ? "unknown" : cpu,
        sys_version
    );
    /*@=usedef@*/
    /*@=compdef@*/

    result = TRUE;

ERR:
    g_free(cpu);
    g_free(sys_version);
    return result;
}

int
cmerge_info_action(
    CPContext ctx,
    /*@unused@*/ const CMergeOptions options G_GNUC_UNUSED,
    GError **error
) {
    struct utsname utsname;
    CPRepository main_repo;
    const char *portdir;
    char *linker = NULL;
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

    print_version(ctx->settings, &utsname, portdir);

    if (!print_system_name(ctx, &utsname, error)) {
        goto ERR;
    }

    print_porttree_timestamp(portdir);

    linker = get_command_output("ld --version");
    if (linker) {
	    g_print("ld %s\n", linker);
    }
    g_free(linker);

    if (!print_packages(portdir, ctx, error)) {
        goto ERR;
    }
    print_repositories(ctx->settings);
    print_settings(ctx->settings, portdir);

    return EXIT_SUCCESS;

ERR:
    return EXIT_FAILURE;
}
