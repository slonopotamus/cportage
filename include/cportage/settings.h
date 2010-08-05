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

/**
 * Reads configuration data and stores it in a #CPortageSettings structure.
 *
 * @param config_root path to configuration files root dir (typically "/")
 * @param error       return location for a #GError, or %NULL
 * @return a #CPortageSettings structure, free it using cportage_settings_unref()
 */
/*@newref@*/ /*@null@*/ CPortageSettings
cportage_settings_new(
    const char *config_root,
    /*@null@*/ GError **error
) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT /*@modifies *error,errno@*/;

/**
 * Increases reference count of @self by 1.
 *
 * @param self a #CPortageSettings
 * @return @self
 */
/*@newref@*/ CPortageSettings
cportage_settings_ref(
    CPortageSettings self
) /*@modifies *self@*/;

/**
 * Decreases reference count of @self by 1. When reference count drops
 * to zero, it frees all the memory associated with the structure.
 *
 * @param self a #CPortageSettings
 */
void
cportage_settings_unref(
    /*@killref@*/ /*@null@*/ CPortageSettings self
) /*@modifies self@*/;

/**
 * @return readonly value of @key variable or @fallback if variable is not set
 */
G_CONST_RETURN /*@observer@*/ char *
cportage_settings_get_default(
    const CPortageSettings self,
    const char *key,
    const char *fallback
) /*@*/;

/**
 * @return readonly value of @key variable or %NULL if variable is not set
 */
G_CONST_RETURN /*@null@*/ /*@observer@*/ char *
cportage_settings_get(const CPortageSettings self, const char *key) /*@*/;

/**
 * @return readonly value of PORTDIR variable
 */
G_CONST_RETURN /*@observer@*/ char *
cportage_settings_get_portdir(const CPortageSettings self) /*@*/;

/**
 * @return readonly absolute path to make.profile dir
 */
G_CONST_RETURN /*@observer@*/ char *
cportage_settings_get_profile(const CPortageSettings self) /*@*/;

/**
 * @return %true if @feature is enabled in FEATURES variable, %false otherwise
 */
bool cportage_settings_has_feature(
    const CPortageSettings self,
    const char *feature
) /*@*/;

#pragma GCC visibility pop

G_END_DECLS

#endif
