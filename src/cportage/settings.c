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

#include <assert.h>
#include <libiberty.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include "cportage/settings.h"
#include "cportage/object_impl.h"

struct settings_entry {
    char * name;
    char * value;
    struct settings_entry * next;
};

struct CPortageSettings {
    struct CPortageObject _;
    char * config_root;
    struct settings_entry * entries;
};

const void * CPortageSettings;

char * cportage_settings_get_default(const void * _self,
                                     const char * key,
                                     const char * _default) {
    struct CPortageSettings * self = cportage_cast(CPortageSettings, _self);
    assert(key);
    struct settings_entry * e = self->entries;
    while (e) {
        if (strcmp(e->name, key) == 0) {
            return xstrdup(e->value);
        }
        e = e->next;
    }
    return _default == NULL ? NULL : strdup(_default);
}

char * cportage_settings_get(const void * _self, const char * key) {
    return cportage_settings_get_default(_self, key, NULL);
}

char * cportage_settings_get_portdir(const void * _self) {
    return cportage_settings_get_default(_self, "PORTDIR", "/usr/portage");
}

char * cportage_settings_get_profile(const void * _self) {
    struct CPortageSettings * self = cportage_cast(CPortageSettings, _self);
    return concat(self->config_root, "/etc/make.profile", NULL);
}

static void foo(va_list ap) {
  const char * bar = va_arg(ap, char *);
}

static void * Settings_ctor(void * _self, va_list ap) {
    cportage_super_ctor(CPortageSettings, _self, ap);
    struct CPortageSettings * self = cportage_cast(CPortageSettings, _self);
    const char * config_root = va_arg(ap, char *);
    assert(config_root);
    self->config_root = xstrdup(config_root);
    assert(self->config_root);

    // Strip slashes at the end
    ssize_t i = strlen(self->config_root) - 1;
    while (i >= 0) {
        if (self->config_root[i] == '/') {
            self->config_root[i] = '\0';
        }
        --i;
    }

    return self;
}

static void * Settings_dtor(void * _self) {
    struct CPortageSettings * self = cportage_cast(CPortageSettings, _self);
    free(self->config_root);
    return self;
}

void * cportage_initCPortageSettings(void) {
    return cportage_new(CPortageClass, "Settings", CPortageObject, sizeof(struct CPortageSettings),
                        cportage_ctor, Settings_ctor,
                        cportage_dtor, Settings_dtor);
}
