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

#ifndef CPORTAGE_SETTINGS_H
#define CPORTAGE_SETTINGS_H

#include <glib.h>

G_BEGIN_DECLS
#pragma GCC visibility push(default)

typedef /*@abstract@*/ /*@refcounted@*/ struct CPortageSettings *CPortageSettings;

/*@newref@*/ /*@null@*/ CPortageSettings
cportage_settings_new(const char *config_root, /*@out@*/ GError **error);

/*@newref@*/ CPortageSettings
cportage_settings_ref(/*@returned@*/ CPortageSettings self) /*@modifies self@*/;

void
cportage_settings_unref(/*@killref@*/ CPortageSettings self) /*@modifies self@*/;

char *
cportage_settings_get_entry(const CPortageSettings self, const char *key, const char *dflt) /*@*/;

char *
cportage_settings_get_portdir(const CPortageSettings self) /*@*/;

char *
cportage_settings_get_profile(const CPortageSettings self) /*@*/;

#pragma GCC visibility pop

G_END_DECLS

#endif
