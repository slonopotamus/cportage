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

#include "cportage/io.h"
#include "cportage/settings.h"
#include "cportage/shellconfig.h"
#include "cportage/strings.h"

struct CPSettings {
    /*@only@*/ char *config_root;
    /*@only@*/ char *profile;

    /* Stuff below comes from profiles and /etc/portage */

    /*@only@*/ GSList/*<CPAtom>*/ *packages;
    /*@only@*/ GSList/*<CPAtom>*/ *package_mask;
    /*@only@*/ GSList/*<UseItem>*/ *use_force;
    /*@only@*/ GSList/*<UseItem>*/ *use_mask;
    /*@only@*/ GHashTable/*<CPAtom,UseItem>*/ *package_use_mask;

    /*
      Stuff below comes from (in order)
      - /etc/profile.env
      - make.defaults across profiles
      - /etc/make.globals
      - /etc/make.conf
     */

    /*@only@*/ GHashTable/*<char *,char *>*/ *config;
    /*@only@*/ char **features;

    /*@refs@*/ int refs;
};

static bool
cp_settings_load(
    CPSettings self,
    /*@null*/GError **error
) /*@modifies *self, *error@*/ {
    char *make_conf;

    g_assert(error == NULL || *error == NULL);
    g_assert(self->config_root != NULL && self->profile != NULL);
    g_assert(self->config == NULL);

    make_conf = g_build_filename(self->config_root, "etc", "make.conf", NULL);
    self->config = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);

    if (!cp_read_shellconfig(self->config, "/etc/profile.env", false, error)) {
        goto ERR;
    }
    if (!cp_read_shellconfig(self->config, "/etc/make.globals", false, error)) {
        goto ERR;
    }
    if (!cp_read_shellconfig(self->config, make_conf, true, error)) {
        goto ERR;
    }

    g_free(make_conf);
    return true;

ERR:
    g_free(make_conf);
    return false;
}

static void
cp_settings_init_features(CPSettings self) /*@modifies *self@*/ {
    char *key = g_strdup("FEATURES");

    g_assert(self->config != NULL && self->features == NULL);

    self->features = cp_strings_pysplit(cp_settings_get_default(self, key, ""));
    cp_strings_sort(self->features);
    g_hash_table_insert(self->config, key, g_strjoinv(" ", self->features));
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

    self->profile = g_build_filename(self->config_root, "etc", "make.profile", NULL);

    if (!cp_settings_load(self, error)) {
        goto ERR;
    }
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
        g_free(self->profile);

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
cp_settings_get_profile(const CPSettings self) {
    return self->profile;
}

bool cp_settings_has_feature(
    const CPSettings self,
    const char *feature
) {
    g_assert(g_utf8_validate(feature, -1, NULL));

    /* Could be replaced with bsearch since features are sorted */
    CP_STRV_ITER(self->features, f) {
        if (g_strcmp0(f, feature) == 0) {
            return true;
        }
    } end_CP_STRV_ITER

    return false;
}
