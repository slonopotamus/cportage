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

#include "settings.h"

struct CPConfigProtect {
    /*@null@*/ /*@only@*/ GSList/*<char *>*/ *protected;
    /*@null@*/ /*@only@*/ GSList/*<char *>*/ *masked;
};

static /*@null@*/ /*@only@*/ GSList *
build_path_list(CPSettings settings, const char *key) /*@*/ {
    const char *root = cp_settings_root(settings);
    char **items = cp_strings_pysplit(cp_settings_get_default(settings, key, ""));
    GSList *result = NULL;

    CP_STRV_ITER(items, item) {
        result = g_slist_prepend(result, g_build_filename(root, item, NULL));
    } end_CP_STRV_ITER

    g_strfreev(items);
    return result;
}

CPConfigProtect
cp_config_protect_new(CPSettings settings) {
    CPConfigProtect self;

    self = g_new0(struct CPConfigProtect, 1);

    g_assert(self->protected == NULL);
    self->protected = build_path_list(settings, "CONFIG_PROTECT");
    g_assert(self->masked == NULL);
    self->masked = build_path_list(settings, "CONFIG_PROTECT_MASK");

    return self;
}

void
cp_config_protect_destroy(CPConfigProtect self) {
    if (self == NULL) {
        /*@-mustfreeonly@*/
        return;
        /*@=mustfreeonly@*/
    }

    g_slist_free_full(self->protected, g_free);
    g_slist_free_full(self->masked, g_free);

    g_free(self);
}

gboolean
cp_config_protect_is_protected(const CPConfigProtect self, const char *path) {
    /* PMS, section 13.3.3 */

    CP_GSLIST_ITER(self->masked, item) {
        if (strcmp(item, path) == 0 || cp_path_is_descendant(item, path)) {
            return FALSE;
        }
    } end_CP_GSLIST_ITER

    CP_GSLIST_ITER(self->protected, item) {
        if (strcmp(item, path) == 0 || cp_path_is_descendant(item, path)) {
            return TRUE;
        }
    } end_CP_GSLIST_ITER

    return FALSE;
}
