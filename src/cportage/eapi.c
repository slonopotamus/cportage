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

#include "eapi.h"
#include "error.h"
#include "macros.h"
#include "strings.h"

static gboolean
cp_eapi_supported(const char *eapi) /*@*/ {
    return eapi[0] >= '0' && eapi[0] <= '4' && eapi[1] == '\0';
}

gboolean
cp_eapi_check(const char *eapi, const char *file, GError **error) {
    g_assert(error == NULL || *error == NULL);

    if (cp_eapi_supported(eapi)) {
        return TRUE;
    }
    g_set_error(error, CP_ERROR, (gint)CP_ERROR_EAPI_UNSUPPORTED,
        _("Unsupported EAPI '%s' in '%s'"), eapi, file);
    return FALSE;
}

gboolean
cp_eapi_check_file(const char *file, GError **error) {
    char *data;
    gboolean result;

    if (!g_file_test(file, G_FILE_TEST_EXISTS)) {
        return TRUE;
    }

    if (!g_file_get_contents(file, &data, NULL, error)) {
        return FALSE;
    }

    result = cp_eapi_check(g_strstrip(data), file, error);

    g_free(data);
    return result;
}
