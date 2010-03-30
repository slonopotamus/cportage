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

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "cportage/io.h"
#include "cportage/settings.h"

struct CPortageSettings {
    /*@refs@*/ int refs;
    /*@only@*/ char * config_root;
    /*@only@*/ GHashTable * entries;
};

CPortageSettings
cportage_settings_new(const char *config_root, /*@out@*/ GError **error) {
    CPortageSettings self;
    ssize_t i;
    char *make_conf;

    g_assert(error == NULL || *error == NULL);

    self = g_malloc0(sizeof(*self));
    self->refs = 1;

    self->config_root = g_strdup(config_root);
    /* Strip slashes at the end */
    i = strlen(self->config_root) - 1;
    while (i >= 0) {
        if (self->config_root[i] == '/')
            self->config_root[i] = '\0';
        --i;
    }

    self->entries = g_hash_table_new_full(g_str_hash, g_str_equal, free, free);
    make_conf = g_build_path(self->config_root, "etc/make.conf", NULL);
    /*
    cportage_read_shellconfig(make_conf, TRUE, self->entries, error);
    g_assert_no_error(*error);
    */
    free(make_conf);

    return self;
}

CPortageSettings
cportage_settings_ref(CPortageSettings self) {
    ++self->refs;
    return self;
}

void
cportage_settings_unref(CPortageSettings self) {
    g_return_if_fail(self != NULL);
    g_assert(self->refs > 0);
    if (--self->refs == 0) {
        g_hash_table_unref(self->entries);
        free(self->config_root);
        /*@-refcounttrans@*/
        free(self);
        /*@=refcounttrans@*/
    }
}

char *
cportage_settings_get_entry(const CPortageSettings self, const char *key, const char *dflt) {
    const char *retval = g_hash_table_lookup(self->entries, key);
    if (retval == NULL) {
        return g_strdup(dflt);
    } else {
        return g_strdup(retval);
    }
}

char *
cportage_settings_get_portdir(const CPortageSettings self) {
    return cportage_settings_get_entry(self, "PORTDIR", "usr/portage");
}

char *
cportage_settings_get_profile(const CPortageSettings self) {
    return g_build_filename(self->config_root, "etc/make.profile", NULL);
}
