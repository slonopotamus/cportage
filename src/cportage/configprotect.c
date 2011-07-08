/*
    Copyright 2009-2011, Marat Radchenko

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

#include <string.h>

#include <cportage.h>

struct CPConfigProtect {
    /*@only@*/ char **protected;
    /*@only@*/ char **masked;
};

CPConfigProtect
cp_config_protect_new(CPSettings settings) {
    CPConfigProtect self;

    self = g_new0(struct CPConfigProtect, 1);

    /* TODO: only allow absolute paths? */
    self->protected = cp_strings_pysplit(
        cp_settings_get_default(settings, "CONFIG_PROTECT", "")
    );

    self->masked = cp_strings_pysplit(
        cp_settings_get_default(settings, "CONFIG_PROTECT_MASK", "")
    );

    return self;
}

void
cp_config_protect_destroy(CPConfigProtect self) {
    if (self == NULL) {
        /*@-mustfreeonly@*/
        return;
        /*@=mustfreeonly@*/
    }

    g_strfreev(self->protected);
    g_strfreev(self->masked);

    g_free(self);
}

gboolean
cp_config_protect_is_protected(const CPConfigProtect self, const char *path) {
    /* PMS, section 13.3.3 */

    /* TODO: respect settings root */
    CP_STRV_ITER(self->masked, item) {
        if (strcmp(item, path) == 0 || cp_path_is_descendant(item, path)) {
            return FALSE;
        }
    } end_CP_STRV_ITER

    CP_STRV_ITER(self->protected, item) {
        if (strcmp(item, path) == 0 || cp_path_is_descendant(item, path)) {
            return TRUE;
        }
    } end_CP_STRV_ITER

    return FALSE;
}
