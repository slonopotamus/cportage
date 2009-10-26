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

#ifndef CPORTAGE_SETTINGS_H
#define CPORTAGE_SETTINGS_H

#include "cportage/object.h"

#pragma GCC visibility push(default)

void * initSettings();

/* new(Class(Settings), "/path/to/config/root") */
extern const void * Settings;

/* Same as settings_get_default(self, key, NULL) */
char * settings_get(const void * self, const char * key);

/*
	Searches settings value with given key.
	It's caller responsibility to free returned value.
	If no matching entry found, default is returned (strdup'ed if nonnull).
 */
char * settings_get_default(const void * self,
	const char * key,
	const char * _default);

#pragma GCC visibility pop

#endif

