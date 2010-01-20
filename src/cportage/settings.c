/*
    Copyright 2009, Marat Radchenko

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
            return strdup(e->value);
        }
        e = e->next;
    }
    return _default == NULL ? NULL : strdup(_default);
}

char * cportage_settings_get(const void * _self, const char * key) {
    return cportage_settings_get_default(_self, key, NULL);
}

static void * Settings_ctor(void * _self, va_list ap) {
    cportage_super_ctor(CPortageSettings, _self, ap);
    struct CPortageSettings * self = cportage_cast(CPortageSettings, _self);
    const char * config_root = va_arg(ap, char *);
    assert(config_root);
    return self;
}

void * cportage_initCPortageSettings(void) {
    return cportage_new(CPortageClass, "Settings", CPortageObject, sizeof(struct CPortageSettings),
               cportage_ctor, Settings_ctor);
}
