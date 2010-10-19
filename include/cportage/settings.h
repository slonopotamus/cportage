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

#ifndef CP_SETTINGS_H
#define CP_SETTINGS_H

#include <glib.h>

/*@-exportany@*/

G_BEGIN_DECLS

#pragma GCC visibility push(default)

typedef /*@refcounted@*/ struct CPSettings *CPSettings;

/**
 * Reads configuration data and stores it in a #CPSettings structure.
 *
 * @param config_root path to configuration files root dir (typically "/")
 * @param error       return location for a #GError, or %NULL
 * @return a #CPSettings structure, free it using cp_settings_unref()
 */
/*@newref@*/ /*@null@*/ CPSettings
cp_settings_new(
    const char *config_root,
    /*@null@*/ GError **error
) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT /*@modifies *error,errno@*/;

/**
 * Increases reference count of @self by 1.
 *
 * @param self a #CPSettings
 * @return @self
 */
/*@newref@*/ CPSettings
cp_settings_ref(
    CPSettings self
) /*@modifies *self@*/;

/**
 * Decreases reference count of @self by 1. When reference count drops
 * to zero, it frees all the memory associated with the structure.
 *
 * @param self a #CPSettings
 */
void
cp_settings_unref(
    /*@killref@*/ /*@null@*/ CPSettings self
) /*@modifies self@*/;

/**
 * @return readonly value of @key variable or @fallback if variable is not set
 */
G_CONST_RETURN /*@observer@*/ char *
cp_settings_get_default(
    const CPSettings self,
    const char *key,
    const char *fallback
) /*@*/;

/**
 * @return readonly value of @key variable or %NULL if variable is not set
 */
G_CONST_RETURN /*@null@*/ /*@observer@*/ char *
cp_settings_get(const CPSettings self, const char *key) /*@*/;

/**
 * @return readonly value of PORTDIR variable
 */
G_CONST_RETURN /*@observer@*/ char *
cp_settings_get_portdir(const CPSettings self) /*@*/;

/**
 * @return readonly absolute path to make.profile dir
 */
G_CONST_RETURN /*@observer@*/ char *
cp_settings_get_profile_abs_path(const CPSettings self) /*@*/;

/**
 * @return %TRUE if @feature is enabled in FEATURES variable, %FALSE otherwise
 */
gboolean cp_settings_has_feature_enabled(
    const CPSettings self,
    const char *feature
) /*@*/;

#pragma GCC visibility pop

G_END_DECLS

#endif
