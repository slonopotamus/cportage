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
#include "cportage/object.r"

struct settings_entry {
	char * name;
	char * value;
	struct settings_entry * next;
};

struct Settings {
	struct Object _;
	struct settings_entry * entries;
};

const void * Settings;

char * settings_get_default(const void * _self,
		const char * key,
		const char * _default) {
	struct Settings * self = cast(Settings, _self);
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

char * settings_get(const void * _self, const char * key) {
	return settings_get_default(_self, key, NULL);
}

static void * Settings_ctor(void * _self, va_list ap) {
	super_ctor(Settings, _self, ap);
	struct Settings * self = cast(Settings, _self);
	const char * config_root = va_arg(ap, char *);
	assert(config_root);
	return self;
}

void * initSettings() {
	return new(Class, "Settings", Object, sizeof(struct Settings),
		ctor, Settings_ctor);
}
