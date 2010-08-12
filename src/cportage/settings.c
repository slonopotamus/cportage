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
#include "cportage/strings.h"

struct CPortageSettings {
    /*@refs@*/ int refs;

    /*@only@*/ char *config_root;
    /*@only@*/ char *profile;

    /* Stuff below comes from profiles and /etc/portage */

    /*@only@*/ GSList/*<CPortageAtom>*/ *packages;
    /*@only@*/ GSList/*<CPortageAtom>*/ *package_mask;
    /*@only@*/ GSList/*<UseItem>*/ *use_force;
    /*@only@*/ GSList/*<UseItem>*/ *use_mask;
    /*@only@*/ GHashTable/*<CPortageAtom,UseItem>*/ *package_use_mask;

    /*
      Stuff below comes from (in order)
      - /etc/profile.env
      - make.defaults across profiles
      - /etc/make.globals
      - /etc/make.conf
     */

    /*@only@*/ GHashTable/*<char *,char *>*/ *config;
    /*@only@*/ char **features;
};

CPortageSettings
cportage_settings_new(const char *config_root, GError **error) {
    CPortageSettings self = g_new0(struct CPortageSettings, 1);

    g_assert(error == NULL || *error == NULL);
    g_assert(g_utf8_validate(config_root, -1, NULL));

    self->refs = 1;
    self->config_root = cportage_canonical_path(config_root, error);
    if (self->config_root == NULL) {
        goto ERR;
    }

    self->profile = g_build_filename(self->config_root, "etc", "make.profile", NULL);
    self->config = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);

    {
        char *make_conf = g_build_filename(self->config_root, "etc", "make.conf", NULL);
        if (!cportage_read_shellconfig(self->config, "/etc/make.globals", false, error)) {
            goto ERR;
        }
        if (!cportage_read_shellconfig(self->config, make_conf, true, error)) {
            goto ERR;
        }
        g_free(make_conf);
    }
    self->features = cportage_strings_pysplit(
        cportage_settings_get_default(self, "FEATURES", "")
    );
     /*@=mustfreeonly@*/
    cportage_strings_sort(self->features);

    return self;

ERR:
    cportage_settings_unref(self);
    return NULL;
}

CPortageSettings
cportage_settings_ref(CPortageSettings self) {
    ++self->refs;
    return self;
}

void
cportage_settings_unref(CPortageSettings self) {
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
cportage_settings_get_default(
    const CPortageSettings self,
    const char *key,
    const char *fallback
) {
    const char *result = cportage_settings_get(self, key);
    return result == NULL ? fallback : result;
}

G_CONST_RETURN char *
cportage_settings_get(const CPortageSettings self, const char *key) {
    return g_hash_table_lookup(self->config, key);
}

G_CONST_RETURN char *
cportage_settings_get_portdir(const CPortageSettings self) {
    return cportage_settings_get_default(self, "PORTDIR", "/usr/portage");
}

G_CONST_RETURN char *
cportage_settings_get_profile(const CPortageSettings self) {
    return self->profile;
}

bool cportage_settings_has_feature(
    const CPortageSettings self,
    const char *feature
) {
    g_assert(g_utf8_validate(feature, -1, NULL));

    CPORTAGE_STRV_ITER(self->features, f) {
        if (g_strcmp0(f, feature) == 0) {
            return true;
        }
    } end_CPORTAGE_STRV_ITER

    return false;
}
