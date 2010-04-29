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

struct CPortageSettings {
    /*@refs@*/ int refs;

    /*@only@*/ char *config_root;
    GHashTable *entries;
    /*@only@*/ char *portdir;
    /*@only@*/ char *profile;
};

CPortageSettings
cportage_settings_new(const char *config_root, GError **error) {
    CPortageSettings self;
    const char *portdir;

    g_assert(error == NULL || *error == NULL);
    g_assert(g_utf8_validate(config_root, -1, NULL));

    self = g_new(struct CPortageSettings, 1);
    self->refs = 1;

    /*
    make_conf = g_build_filename(config_root, "etc", "make.conf", NULL);
    cportage_read_shellconfig(make_conf, true, self->entries, error);
    g_assert_no_error(*error);
    g_free(make_conf);
    */

    /*@-mustfreeonly@*/
    self->config_root = g_strdup(config_root);
    self->entries = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
    portdir = cportage_settings_get(self, "PORTDIR");
    if (portdir == NULL) {
        self->portdir = g_build_filename("/", "usr", "portage", NULL);
    } else {
        self->portdir = g_strdup(portdir);
    }
    self->profile = g_build_filename(config_root, "etc", "make.profile", NULL);
    /*@=mustfreeonly@*/

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
        g_free(self->config_root);
        /*@-mustfreeonly@*/
        g_hash_table_unref(self->entries);
        self->entries = NULL;
        /*@=mustfreeonly@*/
        g_free(self->portdir);
        g_free(self->profile);
        /*@-refcounttrans@*/
        g_free(self);
        /*@=refcounttrans@*/
    }
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
