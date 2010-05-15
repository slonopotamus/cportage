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
#include <stdbool.h>

/*@-exportany@*/

G_BEGIN_DECLS

#pragma GCC visibility push(default)

typedef /*@refcounted@*/ struct CPortageSettings *CPortageSettings;

/*@newref@*/ /*@null@*/ CPortageSettings
cportage_settings_new(
    const char *config_root,
    /*@null@*/ GError **error
) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT /*@modifies *error@*/;

/*@newref@*/ CPortageSettings
cportage_settings_ref(
    CPortageSettings self
) /*@modifies *self@*/;

void
cportage_settings_unref(
    /*@killref@*/ /*@null@*/ CPortageSettings self
) /*@modifies self@*/;

G_CONST_RETURN /*@observer@*/ char *
cportage_settings_get_default(
    const CPortageSettings self,
    const char *key,
    const char *fallback
) /*@*/;

G_CONST_RETURN /*@null@*/ /*@observer@*/ char *
cportage_settings_get(const CPortageSettings self, const char *key) /*@*/;

G_CONST_RETURN /*@observer@*/ char *
cportage_settings_get_portdir(const CPortageSettings self) /*@*/;

G_CONST_RETURN /*@observer@*/ char *
cportage_settings_get_profile(const CPortageSettings self) /*@*/;

bool cportage_settings_has_feature(
    const CPortageSettings self,
    const char *feature
) /*@*/;

#pragma GCC visibility pop

G_END_DECLS

#endif
