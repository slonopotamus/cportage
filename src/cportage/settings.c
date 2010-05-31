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

    GHashTable *entries;
    /*@observer@*/ const char *portdir;
    /*@only@*/ char *profile;
    /*@only@*/ char **features;
};

CPortageSettings
cportage_settings_new(const char *config_root, GError **error) {
    CPortageSettings self;
    GHashTable *entries;

    g_assert(error == NULL || *error == NULL);
    g_assert(g_utf8_validate(config_root, -1, NULL));

    entries = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);

    {
        char *make_globals = g_build_filename(config_root, "etc", "make.globals", NULL);
        char *make_conf = g_build_filename(config_root, "etc", "make.conf", NULL);
        if (!cportage_read_shellconfig(entries, make_globals, true, error)) {
            g_hash_table_destroy(entries);
            return NULL;
        }
        if (!cportage_read_shellconfig(entries, make_conf, true, error)) {
            g_hash_table_destroy(entries);
            return NULL;
        }
        g_free(make_globals);
        g_free(make_conf);
    }

    self = g_new(struct CPortageSettings, 1);
    self->refs = 1;

    /*@-mustfreeonly@*/
    self->entries = entries;
    self->portdir = cportage_settings_get_default(self, "PORTDIR", "/usr/portage");
    self->profile = g_build_filename(config_root, "etc", "make.profile", NULL);

    self->features = cportage_strings_pysplit(
        cportage_settings_get_default(self, "FEATURES", "")
    );
     /*@=mustfreeonly@*/
    cportage_strings_sort(self->features);

    return self;
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
    g_assert_cmpint(self->refs, >, 0);
    if (--self->refs == 0) {
        /*@-mustfreeonly@*/
        g_hash_table_destroy(self->entries);
        self->entries = NULL;
        /*@=mustfreeonly@*/
        g_free(self->profile);
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
    g_assert(g_utf8_validate(key, -1, NULL));
    return g_hash_table_lookup(self->entries, key);
}

G_CONST_RETURN char *
cportage_settings_get_portdir(const CPortageSettings self) {
    return self->portdir;
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
