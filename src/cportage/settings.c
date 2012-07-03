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

#include <string.h>

#include "collections.h"
#include "eapi.h"
#include "error.h"
#include "incrementals.h"
#include "path.h"
#include "repository.h"
#include "settings.h"
#include "shellconfig.h"
#include "strings.h"

struct CPSettingsS {
    /*@only@*/ char *root;
    /*@only@*/ char *profile;

    /*@owned@*/ GTree/*<char *, char *>*/ *config;
    /*@only@*/ CPIncrementals incrementals;

    CPRepository main_repo;
    /*@only@*/ GSList/*<CPRepository>*/ *repos;
    /*@only@*/ GTree/*<char *,CPRepository>*/ *name2repo;

    /*@refs@*/ unsigned int refs;
};

/**
 * TODO: PMS reference?
 * TODO: documentation.
 */
static void
init_cbuild(CPSettings self) /*@modifies *self@*/ {
    const char *chost;
    if (cp_settings_get(self, "CBUILD") != NULL) {
        return;
    }
    chost = cp_settings_get(self, "CHOST");
    if (chost == NULL) {
        return;
    }
    g_tree_insert(self->config, g_strdup("CBUILD"), g_strdup(chost));
}

static gboolean G_GNUC_WARN_UNUSED_RESULT
read_config(
    CPSettings self,
    const char *path,
    gboolean allow_source,
    gboolean stack_use_expand,
    /*@null@*/ GError **error
) /*@modifies *self,*error,errno@*/ /*@globals fileSystem@*/ {

    g_assert(error == NULL || *error == NULL);

    if (!cp_read_shellconfig(
        self->config,
        (CPShellconfigLookupFunc)g_tree_lookup,
        (CPShellconfigSaveFunc)g_tree_insert,
        path,
        allow_source,
        error)
    ) {
        return FALSE;
    }

    cp_incrementals_config_changed(self->incrementals, stack_use_expand);

    return TRUE;
}

/**
 * Loads profiles (recursively, in a depth-first order)
 * into a #CPSettings structure.
 *
 * \param self        a #CPSettings structure
 * \param profile_dir canonical path to profile directory
 * \param error       return location for a %GError, or %NULL
 * \return            %TRUE on success, %FALSE if an error occurred
 */
static gboolean
add_profile(
    CPSettings self,
    const char *profile_dir,
    /*@null@*/ GError **error
) G_GNUC_WARN_UNUSED_RESULT
/*@modifies *self,*error,*stderr,errno@*/
/*@globals fileSystem@*/;

/**
 * Calls cp_settings_add_profile() for each profile in parents file.
 *
 * \param self         a #CPSettings structure
 * \param parents_file file with parent list (must exist)
 * \param error        return location for a %GError, or %NULL
 * \return             %TRUE on success, %FALSE if an error occurred
 */
static gboolean G_GNUC_WARN_UNUSED_RESULT
add_parent_profiles(
    CPSettings self,
    const char *parents_file,
    /*@null@*/ GError **error
) /*@modifies *self,*error,*stderr,errno@*/ /*@globals fileSystem@*/ {
    char *basedir;
    char **parents;
    gboolean result = TRUE;

    g_assert(error == NULL || *error == NULL);

    parents = cp_io_getlines(parents_file, TRUE, error);
    if (parents == NULL) {
        return FALSE;
    }
    basedir = g_path_get_dirname(parents_file);

    CP_STRV_ITER(parents, parent) {
        /* TODO: canonicalize path? */
        char *parent_path = g_path_is_absolute(parent)
            ? g_strdup(parent)
            : g_build_filename(basedir, parent, NULL);
        /* TODO: protect against loop? */
        result = add_profile(self, parent_path, error);
        g_free(parent_path);
        if (!result) {
            break;
         }
    } end_CP_STRV_ITER

    g_free(basedir);
    g_strfreev(parents);
    return result;
}

/** See declaration above. */
static gboolean
add_profile(CPSettings self, const char *profile_dir, GError **error) {
    char *config_file;
    gboolean result = TRUE;

    g_assert(error == NULL || *error == NULL);

    /* Check eapi */
    config_file = g_build_filename(profile_dir, "eapi", NULL);
    result = cp_eapi_check_file(config_file, error);
    g_free(config_file);
    if (!result) {
        return FALSE;
    }

    /* Check whether profile is deprecated */
    config_file = g_build_filename(profile_dir, "deprecated", NULL);
    if (g_file_test(config_file, G_FILE_TEST_EXISTS)) {
        g_warning("Profile %s is deprecated", profile_dir);
        /* TODO: read and print deprecation reason from file */
    }
    g_free(config_file);

    /* Load parents */
    config_file = g_build_filename(profile_dir, "parent", NULL);
    if (g_file_test(config_file, G_FILE_TEST_EXISTS)) {
        result = add_parent_profiles(self, config_file, error);
    }
    g_free(config_file);
    if (!result) {
        return FALSE;
    }

    /* Parse profile configs */

    config_file = g_build_filename(profile_dir, "make.defaults", NULL);
    if (g_file_test(config_file, G_FILE_TEST_EXISTS)) {
        result = read_config(self, config_file, FALSE, TRUE, error);
    }
    g_free(config_file);
    if (!result) {
        return FALSE;
    }

    return cp_incrementals_process_profile(
        self->incrementals, profile_dir, error
    );
}

/**
 * Loads file with given name from \c $CONFIG_ROOT/etc
 * into a #CPSettings structure.
 *
 * \param self         a #CPSettings structure
 * \param name         filename to load
 * \param allow_source if %TRUE, \c source statements are handled in config file
 * \param error        return location for a %GError, or %NULL
 * \return             %TRUE on success, %FALSE if an error occurred
 */
static gboolean G_GNUC_WARN_UNUSED_RESULT
load_etc_config(
    CPSettings self,
    const char *name,
    gboolean allow_source,
    gboolean root_relative,
    gboolean stack_use_expand,
    /*@null@*/ GError **error
) /*@modifies *self,*error,errno@*/ /*@globals fileSystem@*/ {
    char *path;
    gboolean result;

    g_assert(error == NULL || *error == NULL);

    path = g_build_filename(root_relative ? self->root : "/", "etc", name, NULL);
    result = !g_file_test(path, G_FILE_TEST_EXISTS)
        || read_config(self, path, allow_source, stack_use_expand, error);
    g_free(path);
    return result;
}

/**
 * Constructs root profile path and checks its existence.
 *
 * \param root  root dir for config files
 * \param error return location for a %GError, or %NULL
 * \return      canonical profile path on success,
 *              %NULL if an error occurred
 */
static /*@null@*/ char * G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT
build_profile_path(
    const char *root,
    /*@null@*/ GError **error
) /*@modifies *error,errno@*/ /*@globals fileSystem@*/ {
    char *profile;
    char *result;

    g_assert(error == NULL || *error == NULL);
    profile = g_build_filename(root, "etc", "make.profile", NULL);
    result = cp_path_realpath(profile, error);
    g_free(profile);
    return result;
}

/** TODO: documentation */
static gboolean G_GNUC_WARN_UNUSED_RESULT
init_main_repo(
    CPSettings self,
    /*@null@*/ GError **error
) /*@modifies *self,*error,*stderr,errno@*/ /*@globals fileSystem@*/ {
    const char *portdir;
    char *canonical;

    g_assert(error == NULL || *error == NULL);

    portdir = cp_settings_get_required(self, "PORTDIR", error);
    if (portdir == NULL) {
        return FALSE;
    }

    canonical = cp_path_realpath(portdir, error);
    if (canonical == NULL) {
        return FALSE;
    }

    self->main_repo = cp_repository_new(canonical);

    g_tree_insert(self->config, g_strdup("PORTDIR"), canonical);

    return TRUE;
}

static void
init_repos(CPSettings self) /*@modifies *self,*stderr,errno@*/ /*@globals fileSystem@*/ {
    const char *main_repo_name;
    const char *path_str;
    char **paths;
    GString *overlay_str;

    g_assert(self->repos == NULL);
    g_assert(self->main_repo != NULL);

    main_repo_name = cp_repository_name(self->main_repo);
    path_str = cp_settings_get_default(self, "PORTDIR_OVERLAY", "");

    paths = cp_strings_pysplit(path_str);
    overlay_str = g_string_new("");

    /*
      Settings object references main_repo both through self->main_repo and
      self->name2repo. So, in order to simplify settings destruction,
      we increment refcount here.
     */
     /*@-mustfreefresh@*/
    g_tree_insert(
        self->name2repo,
        g_strdup(main_repo_name),
        cp_repository_ref(self->main_repo)
    );
    /*@=mustfreefresh@*/
    /*@-refcounttrans@*/
    self->repos = g_slist_prepend(self->repos, self->main_repo);
    /*@=refcounttrans@*/

    CP_STRV_ITER(paths, path) {
        CPRepository repo;
        const char *name;

        if (!g_file_test(path, G_FILE_TEST_IS_DIR)) {
            g_warning("PORTDIR_OVERLAY contains '%s' which is not a directory",
                path);
            continue;
        }

        /* TODO: canonicalize path? */
        repo = cp_repository_new(path);
        name = cp_repository_name(repo);

        if (cp_settings_get_repository(self, name) != NULL) {
            /* TODO: print warning (duplicate repo) */
            cp_repository_unref(repo);
            continue;
        }

        g_tree_insert(self->name2repo, g_strdup(name), repo);
        /*
          self->repos is in reverse order so that packages from repositories
          defined _later_ in PORTDIR_OVERLAY appear _earlier_.
         */
        /*@-kepttrans@*/
        self->repos = g_slist_prepend(self->repos, repo);
        /*@=kepttrans@*/

        if (overlay_str->len > 0) {
            overlay_str = g_string_append_c(overlay_str, ' ');
        }
        overlay_str = g_string_append(overlay_str, path);
    } end_CP_STRV_ITER

    g_strfreev(paths);

    g_tree_insert(self->config,
        g_strdup("PORTDIR_OVERLAY"),
        g_string_free(overlay_str, FALSE)
    );
}

CPSettings
cp_settings_new(const char *root, GTree *defaults, GError **error) {
    CPSettings self;

    g_assert(error == NULL || *error == NULL);

    self = g_new0(struct CPSettingsS, 1);

    /* init basic things */
    self->refs = (unsigned int)1;
    g_assert(self->root == NULL);
    self->root = cp_path_realpath(root, error);
    if (self->root == NULL) {
        goto ERR;
    }

    g_assert(self->profile == NULL);
    self->profile = build_profile_path(self->root, error);
    if (self->profile == NULL) {
        goto ERR;
    }

    /* init self->config and self->incrementals */
    g_assert(self->config == NULL);
    self->config = g_tree_new_full(
        (GCompareDataFunc)strcmp, NULL, g_free, g_free
    );

    g_assert(self->incrementals == NULL);
    self->incrementals = cp_incrementals_new(self->config);

    if (defaults == NULL) {
        if (!load_etc_config(self, "profile.env", FALSE, TRUE, TRUE, error)) {
            goto ERR;
        }
        if (!load_etc_config(self, "make.globals", FALSE, FALSE, TRUE, error)) {
            goto ERR;
        }
    } else {
        cp_tree_insert_all(
            defaults,
            self->config,
            (CPCopyFunc)g_strdup,
            (CPCopyFunc)g_strdup,
            NULL
        );
    }

    if (!add_profile(self, self->profile, error)) {
        goto ERR;
    }
    if (!load_etc_config(self, "make.conf", TRUE, TRUE, FALSE, error)) {
        goto ERR;
    }

    cp_incrementals_config_finished(self->incrementals);

    /*
        We do not support CONFIGROOT and ROOT overriding. Actually,
        we do not need these variables at all, but let's act like portage.
        At least savedconfig.eclass depends on this.
     */
    g_tree_insert(self->config,
        g_strdup("PORTAGE_CONFIGROOT"),
        g_strdup(self->root)
    );
    g_tree_insert(self->config,
        g_strdup("ROOT"),
        g_strdup(self->root)
    );

    /* init repositories */
    g_assert(self->name2repo == NULL);
    self->name2repo = g_tree_new_full(
        (GCompareDataFunc)strcmp,
        NULL,
        g_free,
        (GDestroyNotify)cp_repository_unref
    );
    if (!init_main_repo(self, error)) {
        goto ERR;
    }
    init_repos(self);

    /* init misc stuff */
    init_cbuild(self);

    return self;

ERR:
    /*@-usereleased@*/
    cp_settings_unref(self);
    /*@=usereleased@*/
    return NULL;
}

CPSettings
cp_settings_ref(CPSettings self) {
    ++self->refs;
    /*@-refcounttrans@*/
    return self;
    /*@=refcounttrans@*/
}

void
cp_settings_unref(CPSettings self) {
    /*@-mustfreeonly@*/
    if (self == NULL) {
        return;
    }

    g_assert(self->refs > 0);
    if (--self->refs > 0) {
        return;
    }
    /*@=mustfreeonly@*/

    g_free(self->root);
    g_free(self->profile);

    cp_tree_destroy(self->config);
    cp_incrementals_destroy(self->incrementals);

    cp_tree_destroy(self->name2repo);
    cp_repository_unref(self->main_repo);
    /* Repositories refcount is decremented during name2repo destruction */
    g_slist_free(self->repos);

    /*@-refcounttrans@*/
    g_free(self);
    /*@=refcounttrans@*/
}

CPRepository
cp_settings_main_repository(const CPSettings self) {
    return cp_repository_ref(self->main_repo);
}

GSList *
cp_settings_repositories(const CPSettings self) {
    return self->repos;
}

CPRepository
cp_settings_get_repository(const CPSettings self, const char *name) {
    return g_tree_lookup(self->name2repo, name);
}

const char *
cp_settings_get_default(
    const CPSettings self,
    const char *key,
    const char *fallback
) {
    const char *result = cp_settings_get(self, key);
    return result == NULL ? fallback : result;
}

const char *
cp_settings_get(const CPSettings self, const char *key) {
    return g_tree_lookup(self->config, key);
}

const char *
cp_settings_get_required(
    const CPSettings self,
    const char *key,
    GError **error
) {
    const char *result;

    g_assert(error == NULL || *error == NULL);

    result = cp_settings_get(self, key);
    if (result == NULL) {
        g_set_error(error, CP_ERROR, (gint)CP_ERROR_SETTINGS_REQUIRED_MISSING,
            _("Required config variable '%s' not found"), key);
    }
    return result;
}

const char *
cp_settings_profile(const CPSettings self) {
    return self->profile;
}

const char *
cp_settings_root(const CPSettings self) {
    return self->root;
}

gboolean
cp_settings_feature_enabled(const CPSettings self, const char *feature) {
    return cp_incrementals_contains(self->incrementals, "FEATURES", feature);
}
