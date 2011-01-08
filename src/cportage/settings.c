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

#include <cportage/eapi.h>
#include <cportage/io.h>
#include <cportage/settings.h>
#include <cportage/shellconfig.h>
#include <cportage/strings.h>

struct CPSettings {
    /*@only@*/ char *config_root;
    /*@only@*/ char *target_root;
    /*@only@*/ char *profile;

    /*@only@*/ GHashTable/*<char *,char *>*/ *config;
    /*@only@*/ char **features;

    CPRepository main_repo;
    CPRepository* repos;

    /*@refs@*/ int refs;
};

struct CPRepository {
    /*@only@*/ char *path;

    /*@refs@*/ int refs;
};

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
cp_settings_add_profile(
    CPSettings self,
    const char *profile_dir,
    /*@null@*/ GError **error
) /*@modifies *self, *error@*/ G_GNUC_WARN_UNUSED_RESULT;

/**
 * Calls cp_settings_add_profile() for each profile in parents file.
 *
 * \param self         a #CPSettings structure
 * \param parents_file file with parent list (must exist)
 * \param error        return location for a %GError, or %NULL
 * \return             %TRUE on success, %FALSE if an error occurred
 */
static gboolean G_GNUC_WARN_UNUSED_RESULT
cp_settings_add_parent_profiles(
    CPSettings self,
    const char *parents_file,
    /*@null@*/ GError **error
) {
    char *basedir;
    char **parents;
    gboolean result = TRUE;

    g_assert(error == NULL || *error == NULL);

    parents = cp_read_lines(parents_file, TRUE, error);
    if (parents == NULL) {
        return FALSE;
    }
    basedir = g_path_get_dirname(parents_file);

    CP_STRV_ITER(parents, parent)
        /* TODO: canonicalize path? */
        char *parent_path = g_path_is_absolute(parent)
            ? g_strdup(parent)
            : g_build_filename(basedir, parent, NULL);
        /* TODO: protect against loop? */
        result = cp_settings_add_profile(self, parent_path, error);
        g_free(parent_path);
        if (!result) {
            break;
         }
    end_CP_STRV_ITER

    g_free(basedir);
    g_strfreev(parents);
    return result;
}

/** See declaration above. */
static gboolean
cp_settings_add_profile(
    CPSettings self,
    const char *profile_dir,
    GError **error
) {
    char *config_file;
    gboolean result = TRUE;

    g_assert(error == NULL || *error == NULL);

    /* Check eapi */
    config_file = g_build_filename(profile_dir, "eapi", NULL);
    if (g_file_test(config_file, G_FILE_TEST_EXISTS)) {
        char *data;
        result = cp_read_file(config_file, &data, NULL, error);
        if (result) {
            result = cp_eapi_check(g_strstrip(data), config_file, error);
        }
        g_free(data);
    }
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
    if (!result) {
        return FALSE;
    }

    /* Load parents */
    config_file = g_build_filename(profile_dir, "parent", NULL);
    if (g_file_test(config_file, G_FILE_TEST_EXISTS)) {
        result = cp_settings_add_parent_profiles(self, config_file, error);
    }
    g_free(config_file);
    if (!result) {
        return FALSE;
    }

    /* Parse profile configs */
    config_file = g_build_filename(profile_dir, "make.defaults", NULL);
    if (g_file_test(config_file, G_FILE_TEST_EXISTS)) {
        result = cp_read_shellconfig(self->config, config_file, FALSE, error);
    }
    g_free(config_file);

    return result;
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
cp_settings_load_etc_config(
    CPSettings self,
    const char *name,
    gboolean allow_source,
    /*@null@*/ GError **error
) {
    char *path;
    gboolean result;

    g_assert(error == NULL || *error == NULL);
    path = g_build_filename(self->config_root, "etc", name, NULL);
    result = cp_read_shellconfig(self->config, path, allow_source, error);
    g_free(path);
    return result;
}

/**
 * Constructs root profile path and checks its existence.
 *
 * \param config_root  root dir for config files
 * \param error        return location for a %GError, or %NULL
 * \return             canonical profile path on success,
 *                     %NULL if an error occurred
 */
static char * G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT
cp_settings_build_profile_path(
    const char *config_root,
    /*@null@*/ GError **error
) /*@modifies *self@*/ {
    char *profile;
    char *result;

    g_assert(error == NULL || *error == NULL);
    profile = g_build_filename(config_root, "etc", "make.profile", NULL);
    result = cp_canonical_path(profile, error);
    g_free(profile);
    return result;
}

/**
 * TODO: PMS reference?
 * TODO: documentation.
 */
static void
cp_settings_init_cbuild(CPSettings self) /*@modifies *self@*/ {
    static const char *key = "CBUILD";
    const char *chost;
    if (cp_settings_get(self, key) != NULL) {
        return;
    }
    chost = cp_settings_get(self, "CHOST");
    if (chost == NULL) {
        return;
    }
    g_hash_table_insert(self->config, g_strdup(key), g_strdup(chost));
}

/**
 * Populates #CPSettings feature list with data from \c FEATURES config variable.
 *
 * \param self a #CPSettings structure
 */
static void
cp_settings_init_features(CPSettings self) /*@modifies *self@*/ {
    static const char *key = "FEATURES";
    self->features = cp_strings_pysplit(cp_settings_get_default(self, key, ""));
    cp_strings_sort(self->features);
    g_hash_table_insert(self->config,
        g_strdup(key),
        g_strjoinv(" ", self->features));
}

/** TODO: documentation */
static gboolean G_GNUC_WARN_UNUSED_RESULT
cp_settings_init_main_repo(CPSettings self, GError **error) {
    const char *portdir;
    char *canonical;

    g_assert(error == NULL || *error == NULL);

    portdir = cp_settings_get_required(self, "PORTDIR", error);
    if (portdir == NULL) {
        return FALSE;
    }

    canonical = cp_canonical_path(portdir, error);
    if (canonical == NULL) {
        return FALSE;
    }

    self->main_repo = cp_repository_new(canonical);

    g_hash_table_insert(self->config, g_strdup("PORTDIR"), canonical);

    return TRUE;
}

static void
cp_settings_init_repos(const CPSettings self, /*@null@*/ GError **error) {
    int i = 0;
    GString *overlays;

    g_assert(error == NULL || *error == NULL);

    self->repos = g_new0(CPRepository, 2);
    self->repos[0] = cp_repository_ref(self->main_repo);

    overlays = g_string_new("");
    CP_REPOSITORY_ITER(self->repos, repo)
        if (repo != self->main_repo) {
            g_string_append_printf(overlays,
                "%s%s", i++ > 0 ? " " : "", repo->path);
        }
    end_CP_REPOSITORY_ITER

    g_hash_table_insert(self->config,
        g_strdup("PORTDIR_OVERLAY"),
        overlays->str
    );
    g_string_free(overlays, FALSE);
}

CPSettings
cp_settings_new(
    const char *config_root,
    const char *target_root,
    GError **error
) {
    CPSettings self;

    g_assert(error == NULL || *error == NULL);

    self = g_new0(struct CPSettings, 1);

    /* init basic things */
    self->refs = 1;
    self->config_root = cp_canonical_path(config_root, error);
    if (self->config_root == NULL) {
        goto ERR;
    }
    self->target_root = cp_canonical_path(target_root, error);
    if (self->target_root == NULL) {
        goto ERR;
    }
    self->profile = cp_settings_build_profile_path(self->config_root, error);
    if (self->profile == NULL) {
        goto ERR;
    }

    /* init self->config */
    self->config = g_hash_table_new_full(
        g_str_hash, g_str_equal, g_free, g_free
    );
    if (!cp_settings_load_etc_config(self, "profile.env", FALSE, error)) {
        goto ERR;
    }
    if (!cp_settings_load_etc_config(self, "make.globals", FALSE, error)) {
        goto ERR;
    }
    if (!cp_settings_add_profile(self, self->profile, error)) {
        goto ERR;
    }
    if (!cp_settings_load_etc_config(self, "make.conf", TRUE, error)) {
        goto ERR;
    }
    /*
        We do not support CONFIGROOT and ROOT overriding. Actually,
        we do not need these variables at all, but let's act like portage.
     */
    g_hash_table_insert(self->config,
        g_strdup("PORTAGE_CONFIGROOT"),
        g_strdup(self->config_root)
    );
    g_hash_table_insert(self->config,
        g_strdup("ROOT"),
        g_strdup(self->target_root)
    );

    /* init repositories */
    if (!cp_settings_init_main_repo(self, error)) {
        goto ERR;
    }
    cp_settings_init_repos(self, error);

    /* init misc stuff */
    cp_settings_init_cbuild(self);
    cp_settings_init_features(self);

    return self;

ERR:
    cp_settings_unref(self);
    return NULL;
}

CPSettings
cp_settings_ref(CPSettings self) {
    ++self->refs;
    return self;
}

void
cp_settings_unref(CPSettings self) {
    if (self == NULL) {
        /*@-mustfreeonly@*/
        return;
        /*@=mustfreeonly@*/
    }
    g_assert(self->refs > 0);
    if (--self->refs == 0) {
        g_free(self->config_root);
        g_free(self->target_root);
        g_free(self->profile);
        if (self->config != NULL) {
            g_hash_table_destroy(self->config);
        }
        g_strfreev(self->features);
        cp_repository_unref(self->main_repo);
        if (self->repos != NULL) {
            CP_REPOSITORY_ITER(self->repos, repo)
                cp_repository_unref(repo);
            end_CP_REPOSITORY_ITER
        }
        g_free(self->repos);

        /*@-refcounttrans@*/
        g_free(self);
        /*@=refcounttrans@*/
    }
}

CPRepository
cp_settings_get_main_repository(const CPSettings self) {
    return self->main_repo;
}

CPRepository*
cp_settings_get_repositories(const CPSettings self) {
    return self->repos;
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
    return g_hash_table_lookup(self->config, key);
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
        g_set_error(error, CP_SETTINGS_ERROR, CP_SETTINGS_ERROR_REQUIRED,
            _("Required config variable '%s' not found"), key);
    }
    return result;
}

const char *
cp_settings_get_profile(const CPSettings self) {
    return self->profile;
}

gboolean
cp_settings_has_feature_enabled(const CPSettings self, const char *feature) {
    /* Could be replaced with bsearch since features are sorted */
    CP_STRV_ITER(self->features, f) {
        if (g_strcmp0(f, feature) == 0) {
            return TRUE;
        }
    } end_CP_STRV_ITER

    return FALSE;
}

GQuark
cp_settings_error_quark(void) {
  return g_quark_from_static_string("cp-settings-error-quark");
}
