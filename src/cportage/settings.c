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

#include "cportage/eapi.h"
#include "cportage/io.h"
#include "cportage/settings.h"
#include "cportage/shellconfig.h"
#include "cportage/strings.h"

struct CPSettings {
    /*@only@*/ char *config_root;
    /*@only@*/ char *profile_abs_path;

    /* Stuff below comes from profiles and /etc/portage */

    /*@only@*/ GSList/*<CPAtom>*/ *packages;
    /*@only@*/ GSList/*<CPAtom>*/ *package_mask;
    /*@only@*/ GSList/*<UseItem>*/ *use_force;
    /*@only@*/ GSList/*<UseItem>*/ *use_mask;
    /*@only@*/ GHashTable/*<CPAtom,UseItem>*/ *package_use_mask;

    /*
      Stuff below comes from (in order)
      - /etc/profile.env
      - /etc/make.globals
      - make.defaults across profiles
      - /etc/make.conf
     */

    /*@only@*/ GHashTable/*<char *,char *>*/ *config;
    /*@only@*/ char **features;

    /*@refs@*/ int refs;
};

/**
 * Loads profiles (recursively, in a depth-first order)
 * into a #CPSettings structure.
 *
 * @param self        a #CPSettings structure
 * @param profile_dir canonical path to profile directory
 * @param error       return location for a #GError, or %NULL
 * @return            %TRUE on success, %FALSE if an error occurred
 */
static gboolean
cp_settings_add_profile(
    CPSettings self,
    const char *profile_dir,
    /*@null@*/ GError **error
) /*@modifies *self, *error@*/ G_GNUC_WARN_UNUSED_RESULT;


/**
 * Calls #cp_settings_add_profile for each prifile in parents file.
 *
 * @param self         a #CPSettings structure
 * @param parents_file file with parent list (must exist)
 * @param error        return location for a #GError, or %NULL
 * @return             %TRUE on success, %FALSE if an error occurred
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
        /* Canonicalize path? */
        char *parent_path = g_path_is_absolute(parent)
            ? g_strdup(parent)
            : g_build_filename(basedir, parent, NULL);
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
        /* TODO: read and print file contents */
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
 * Loads file with given name from $CONFIG_ROOT/etc into a #CPSettings structure.
 *
 * @param self         a #CPSettings structure
 * @param name         filename to load
 * @param allow_source if %TRUE, 'source' statements are handled in config file
 * @param error        return location for a #GError, or %NULL
 * @return             %TRUE on success, %FALSE if an error occurred
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
 * @param config_root  root dir for config files
 * @param error        return location for a #GError, or %NULL
 * @return             canonical profile path on success,
 *                     %NULL if an error occurred
 */
static char *
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

/** TODO: PMS reference? */
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
 * Populates #CPSettings feature list with data from FEATURES config variable.
 *
 * @param self a #CPSettings structure
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

CPSettings
cp_settings_new(const char *config_root, GError **error) {
    CPSettings self;

    g_assert(error == NULL || *error == NULL);
    g_assert(g_utf8_validate(config_root, -1, NULL));

    self = g_new0(struct CPSettings, 1);

    self->refs = 1;
    self->config_root = cp_canonical_path(config_root, error);
    if (self->config_root == NULL) {
        goto ERR;
    }
    self->profile_abs_path = cp_settings_build_profile_path(config_root, error);
    if (self->profile_abs_path == NULL) {
        goto ERR;
    }
    self->config = g_hash_table_new_full(g_str_hash, g_str_equal,g_free, g_free);
    if (!cp_settings_load_etc_config(self, "profile.env", FALSE, error)) {
        goto ERR;
    }
    if (!cp_settings_load_etc_config(self, "make.globals", FALSE, error)) {
        goto ERR;
    }
    if (!cp_settings_add_profile(self, self->profile_abs_path, error)) {
        goto ERR;
    }
    if (!cp_settings_load_etc_config(self, "make.conf", TRUE, error)) {
        goto ERR;
    }

    /*
        We do not support CONFIGROOT overriding. Actually, we do not need this
        at all, but let's act like portage.
     */
    g_hash_table_insert(self->config,
        g_strdup("PORTAGE_CONFIGROOT"),
        g_strdup(self->config_root));

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
        g_free(self->profile_abs_path);
        if (self->package_use_mask != NULL) {
            g_hash_table_destroy(self->package_use_mask);
        }
        if (self->config != NULL) {
            g_hash_table_destroy(self->config);
        }
        g_strfreev(self->features);

        /*@-refcounttrans@*/
        g_free(self);
        /*@=refcounttrans@*/
    }
}

G_CONST_RETURN char *
cp_settings_get_default(
    const CPSettings self,
    const char *key,
    const char *fallback
) {
    const char *result = cp_settings_get(self, key);
    return result == NULL ? fallback : result;
}

G_CONST_RETURN char *
cp_settings_get(const CPSettings self, const char *key) {
    return g_hash_table_lookup(self->config, key);
}

G_CONST_RETURN char *
cp_settings_get_portdir(const CPSettings self) {
    return cp_settings_get_default(self, "PORTDIR", "/usr/portage");
}

G_CONST_RETURN char *
cp_settings_get_profile_abs_path(const CPSettings self) {
    return self->profile_abs_path;
}

gboolean cp_settings_has_feature_enabled(
    const CPSettings self,
    const char *feature
) {
    g_assert(g_utf8_validate(feature, -1, NULL));

    /* Could be replaced with bsearch since features are sorted */
    CP_STRV_ITER(self->features, f) {
        if (g_strcmp0(f, feature) == 0) {
            return TRUE;
        }
    } end_CP_STRV_ITER

    return FALSE;
}
