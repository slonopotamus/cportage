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

#include <stdlib.h>
#include <string.h>

#if HAVE_LOCALE_H
#   include <locale.h>
#endif

#include <cportage.h>

static /*@observer@*/ const char *
get_root(void) /*@*/ {
    const char *result = g_getenv("ROOT");
    return result == NULL ? "/" : result;
}

static int
envvar(
    int argc,
    char **argv,
    GError **error
) /*@modifies *error,*stdout,*stderr,errno@*/ /*@globals fileSystem@*/ {
    int i = 0;
    gboolean verbose = FALSE;
    CPSettings settings = NULL;
    int retval = 2;

    if (argc > 0 && strcmp("-v", argv[0]) == 0) {
        verbose = TRUE;
        ++i;
    }

    if (argc < 1) {
        g_critical(_("ERROR: insufficient parameters!"));
        goto ERR;
    }

    settings = cp_settings_new(get_root(), error);
    if (settings == NULL) {
        goto ERR;
    }

    for (; i < argc; ++i) {
        const char *value = cp_settings_get_default(settings, argv[i], "");
        if (verbose) {
            g_print("%s='%s'\n", argv[i], value);
        } else {
            g_print("%s\n", value);
        }
    }

    retval =  EXIT_SUCCESS;

ERR:
    cp_settings_unref(settings);

    return retval;
}

static int
get_repos(
    int argc,
    char **argv,
    GError **error
) /*@modifies *error,*stdout,*stderr,errno@*/ /*@globals fileSystem@*/ {
    CPSettings settings = NULL;
    int i = 0;
    int retval = 2;

    if (argc < 1) {
        g_critical(_("ERROR: insufficient parameters!"));
        goto ERR;
    }

    settings = cp_settings_new(argv[0], error);
    if (settings == NULL) {
        goto ERR;
    }

    CP_GSLIST_ITER(cp_settings_repositories(settings), repo) {
        if (i++ > 0) {
            g_print(" ");
        }
        g_print("%s", cp_repository_name(repo));
    } end_CP_GSLIST_ITER

    g_print("\n");

    retval = EXIT_SUCCESS;

ERR:
    cp_settings_unref(settings);

    return retval;
}

static int
get_repo_path(
    int argc, 
    char **argv,
    GError **error
) /*@modifies *error,*stdout,*stderr,errno@*/ /*@globals fileSystem@*/ {
    CPSettings settings = NULL;
    int i;
    int retval = 2;

    if (argc < 2) {
        g_critical(_("ERROR: insufficient parameters!"));
        goto ERR;
    }

    settings = cp_settings_new(argv[0], error);
    if (settings == NULL) {
        goto ERR;
    }

    for (i = 1; i < argc; ++i) {
        CPRepository repo = cp_settings_get_repository(settings, argv[i]);
        g_print("%s\n", repo == NULL ? "": cp_repository_path(repo));
        cp_repository_unref(repo);
    }

    retval = EXIT_SUCCESS;

ERR:
    cp_settings_unref(settings);

    return retval;
}

static int
do_with_pkgs(
    int argc,
    char **argv,
    int (*func)(GSList *pkgs),
    /*@null@*/ GError **error
) /*@modifies *error,*stderr,errno@*/ /*@globals fileSystem@*/ {
    CPAtom atom = NULL;
    CPSettings settings = NULL;
    CPVartree vartree = NULL;
    CPTree vardb = NULL;
    GSList *pkgs = NULL;
    int retval = 2;

    if (argc != 2) {
        g_critical(_("ERROR: expected 2 parameters, got %d!"), argc);
        goto ERR;
    }

    atom = cp_atom_new(argv[1], error);
    if (atom == NULL) {
        goto ERR;
    }

    settings = cp_settings_new(argv[0], error);
    if (settings == NULL) {
        goto ERR;
    }

    vartree = cp_vartree_new(settings, error);
    if (vartree == NULL) {
        goto ERR;
    }
    vardb = cp_vartree_get_tree(vartree);

    if (!cp_tree_find_packages(vardb, atom, &pkgs, error)) {
        goto ERR;
    }

    retval = func(pkgs);

ERR:
    cp_package_list_free(pkgs);
    cp_atom_unref(atom);
    cp_tree_unref(vardb);
    cp_vartree_unref(vartree);
    cp_settings_unref(settings);

    return retval;
}

static int G_GNUC_WARN_UNUSED_RESULT
print_pkgs(GSList *pkgs) /*@modifies *stdout,errno@*/ {
    CP_GSLIST_ITER(pkgs, pkg) {
        g_print("%s\n", cp_package_str(pkg));
    } end_CP_GSLIST_ITER

    return EXIT_SUCCESS;
}

static int G_GNUC_WARN_UNUSED_RESULT
test_nonempty(GSList *pkgs) /*@*/ {
    return pkgs == NULL? 1 : EXIT_SUCCESS;
}

static int G_GNUC_WARN_UNUSED_RESULT
print_last(GSList *pkgs) /*@modifies *stdout,errno@*/ {
    GSList *last = g_slist_last(pkgs);

    if (last == NULL) {
        g_print("\n");
    } else {
        g_assert(last->data != NULL);
        g_print("%s\n", cp_package_str(last->data));
    }

    return EXIT_SUCCESS;
}

static int
vdb_path(
    GError **error
) /*@modifies *error,*stdout,*stderr,errno@*/ /*@globals fileSystem@*/ {
    CPSettings settings = NULL;
    CPVartree vartree  = NULL;
    int retval = 2;

    settings = cp_settings_new(get_root(), error);
    if (settings == NULL) {
        goto ERR;
    }

    vartree = cp_vartree_new(settings, error);
    if (vartree == NULL) {
        goto ERR;
    }

    g_print("%s\n", cp_vartree_path(vartree));

    retval = EXIT_SUCCESS;

ERR:
    cp_settings_unref(settings);
    cp_vartree_unref(vartree);

    return retval;
}

static int
is_protected(
    int argc,
    char **argv,
    GError **error
) /*@modifies *error,*stderr,errno@*/ /*@globals fileSystem@*/ {
    CPSettings settings = NULL;
    CPConfigProtect config_protect = NULL;
    int retval = 2;

    if (argc != 2) {
        g_critical(_("ERROR: expected 2 parameters, got %d!"), argc);
        goto ERR;
    }

    settings = cp_settings_new(argv[0], error);
    if (settings == NULL) {
        goto ERR;
    }

    config_protect = cp_config_protect_new(settings);

    retval = cp_config_protect_is_protected(config_protect, argv[1])
        ? EXIT_SUCCESS
        : EXIT_FAILURE;

ERR:
    cp_settings_unref(settings);
    cp_config_protect_destroy(config_protect);

    return retval;
}

static void
usage(const char *progname) /*@modifies *stdout,errno@*/ {
    /* TODO: usage docs */
    g_print("%s: usage\n", progname);
}

/*@observer@*/ /*@unchecked@*/ static const char * const vars[] = {
    "distdir", "pkgdir", "portdir_overlay", "config_protect_mask",
    "config_protect", "portdir", "gentoo_mirrors"
};

int
main(int argc, char **argv)
/*@modifies *stdout,*stderr,errno,internalState,fileSystem@*/ {
    GError *error = NULL;
    int retval;

#if HAVE_SETLOCALE
    (void)setlocale(LC_ALL, "");
#endif

    if (argc < 2) {
        usage(argv[0]);
        return 2;
    }

    if (strcmp("--help", argv[1]) == 0 || strcmp("-h", argv[1]) == 0) {
        usage(argv[0]);
        return EXIT_SUCCESS;
    }

    if (strcmp("envvar", argv[1]) == 0) {
        retval = envvar(argc - 2, &argv[2], &error);
    } else if (strcmp("get_repos", argv[1]) == 0) {
        retval = get_repos(argc - 2, &argv[2], &error);
    } else if (strcmp("get_repo_path", argv[1]) == 0) {
        retval = get_repo_path(argc - 2, &argv[2], &error);
    } else if (strcmp("match", argv[1]) == 0) {
        retval = do_with_pkgs(argc - 2, &argv[2], print_pkgs, &error);
    } else if (strcmp("has_version", argv[1]) == 0) {
        retval = do_with_pkgs(argc - 2, &argv[2], test_nonempty, &error);
    } else if (strcmp("best_version", argv[1]) == 0) {
        retval = do_with_pkgs(argc - 2, &argv[2], print_last, &error);
    } else if (strcmp("vdb_path", argv[1]) == 0) {
        retval = vdb_path(&error);
    } else if (strcmp("is_protected", argv[1]) == 0) {
        retval = is_protected(argc - 2, &argv[2], &error);
    /*
      TODO: mass_best_version, metadata, contents, owners,
      filter_protected, best_visible, mass_best_visible, all_best_visible,
      list_preserved_libs
     */
    } else {
        size_t i;

        for (i = 0; i < G_N_ELEMENTS(vars); ++i) {
            char *arr[1];

            if (strcmp(argv[1], vars[i]) != 0) {
               continue;
            }

            arr[0] = g_ascii_strup(argv[1], (ssize_t)-1);
            retval = envvar(1, arr, &error);
            g_free(arr[0]);
            goto OUT;
        }
        usage(argv[0]);
        return 2;
    }

OUT:
    if (retval == EXIT_SUCCESS) {
        return retval;
    }

    if (error != NULL) {
        g_critical("%s", error->message);
        g_error_free(error);
    }

    return retval;
}
