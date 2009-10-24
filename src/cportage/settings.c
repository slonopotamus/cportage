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

#include "cportage/settings.h"
#include "cportage/object.r"

struct Settings {
	struct Object _;
};

static void * Settings_new(const void * _class, va_list ap) {
	const char * config_root = va_arg(ap, char *);
	assert(config_root);
	struct Settings * settings = super_ctor(Settings, alloc(_class), NULL);
	return settings;
}

const void * Settings;

void * initSettings() {
	return new(Class, "Settings", Object, sizeof(struct Settings),
		new, Settings_new);
}
