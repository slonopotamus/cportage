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

#if HAVE_LOCALE_H
#   include <locale.h>
#endif

#include <stdlib.h>
#include <cportage.h>
#include "../cportage/macros.h"

static int
envvar(
    int argc,
    char **argv,
    GError **error
) /*@modifies *error@*/ /*@globals fileSystem@*/ {
    int i = 0;
    gboolean verbose = FALSE;
    CPSettings settings = NULL;
    int retval = 2;

    if (argc > 0 && g_strcmp0("-v", argv[0]) == 0) {
        verbose = TRUE;
        ++i;
    }

    if (argc < 1) {
        g_critical(_("ERROR: insufficient parameters!"));
        goto ERR;
    }

    settings = cp_settings_new("/", error);
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
) /*@modifies *error@*/ /*@globals fileSystem@*/ {
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

    CP_REPOSITORY_ITER(cp_settings_repositories(settings), repo) {
        if (i++ > 0) {
            g_print(" ");
        }
        g_print("%s", cp_repository_name(repo));
    } end_CP_REPOSITORY_ITER

    if (i > 0) {
        g_print("\n");
    }

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
) /*@modifies *error@*/ /*@globals fileSystem@*/ {
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
        g_print("%s\n", repo == NULL ? "None": cp_repository_path(repo));
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
    int (*func)(GList *pkgs),
    /*@null@*/ GError **error
) /*@modifies *error@*/ /*@globals fileSystem@*/ {
    CPAtom atom = NULL;
    CPSettings settings = NULL;
    CPVartree vartree = NULL;
    GList *pkgs = NULL;
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

    vartree = cp_vartree_new(settings);
    if (vartree == NULL) {
        goto ERR;
    }

    if (!cp_vartree_find_packages(vartree, atom, &pkgs, error)) {
        goto ERR;
    }

    retval = func(pkgs);

ERR:
    cp_package_list_free(pkgs);
    cp_atom_unref(atom);
    cp_vartree_unref(vartree);
    cp_settings_unref(settings);

    return retval;
}

static int
print_pkgs(GList *pkgs) {
    CP_GLIST_ITER(pkgs, pkg) {
        g_print("%s\n", cp_package_str(pkg));
    } end_CP_GLIST_ITER

    return EXIT_SUCCESS;
}

static int
test_nonempty(GList *pkgs) {
    return pkgs == NULL? 1 : EXIT_SUCCESS;
}

static int
print_last(GList *pkgs) {
    GList *last = g_list_last(pkgs);

    if (last == NULL) {
        g_print("\n");
    } else {
        g_assert(last->data != NULL);
        g_print("%s\n", cp_package_str(last->data));
    }

    return EXIT_SUCCESS;
}

static void
usage(const char *progname) /*@modifies *stdout@*/ {
    /* TODO: usage docs */
    g_print("%s: usage\n", progname);
}

/*@observer@*/ static const char * const vars[] = {
    "distdir", "pkgdir", "portdir_overlay", "config_protect_mask",
    "config_protect", "portdir", "gentoo_mirrors"
};

int
main(int argc, char **argv)
/*@modifies stdout,*stderr,errno,internalState,fileSystem@*/ /*@globals vars@*/ {
    GError *error = NULL;
    int retval;

#if HAVE_SETLOCALE
    (void)setlocale(LC_ALL, "");
#endif

    if (argc < 2) {
        usage(argv[0]);
        return 2;
    }

    if (g_strcmp0("--help", argv[1]) == 0 || g_strcmp0("-h", argv[1]) == 0) {
        usage(argv[0]);
        return EXIT_SUCCESS;
    }

    if (g_strcmp0("envvar", argv[1]) == 0) {
        retval = envvar(argc - 2, &argv[2], &error);
    } else if (g_strcmp0("get_repos", argv[1]) == 0) {
        retval = get_repos(argc - 2, &argv[2], &error);
    } else if (g_strcmp0("get_repo_path", argv[1]) == 0) {
        retval = get_repo_path(argc - 2, &argv[2], &error);
    } else if (g_strcmp0("match", argv[1]) == 0) {
        retval = do_with_pkgs(argc - 2, &argv[2], print_pkgs, &error);
    } else if (g_strcmp0("has_version", argv[1]) == 0) {
        retval = do_with_pkgs(argc - 2, &argv[2], test_nonempty, &error);
    } else if (g_strcmp0("best_version", argv[1]) == 0) {
        retval = do_with_pkgs(argc - 2, &argv[2], print_last, &error);
    /*
      TODO: mass_best_version, metadata, contents, owners, is_protected,
      filter_protected, best_visible, mass_best_visible, all_best_visible,
      vdb_path, list_preserved_libs
     */
    } else {
        size_t i;

        for (i = 0; i < G_N_ELEMENTS(vars); ++i) {
            char *arr[1];

            if (g_strcmp0(argv[1], vars[i]) != 0) {
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
