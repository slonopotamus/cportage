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

#include <cportage/eapi.h>
#include <cportage/strings.h>

static gboolean
cp_eapi_supported(const char *eapi) {
    return eapi[0] >= '0' && eapi[0] <= '3' && eapi[1] == '\0';
}

gboolean
cp_eapi_check(const char *eapi, const char *file, GError **error) {
    g_assert(error == NULL || *error == NULL);

    if (cp_eapi_supported(eapi)) {
        return TRUE;
    }
    g_set_error(error,
        CP_EAPI_ERROR,
        CP_EAPI_ERROR_UNSUPPORTED,
        _("Unsupported EAPI %s in %s"),
        eapi,
        file
    );
    return FALSE;
}

GQuark
cp_eapi_error_quark(void) {
  return g_quark_from_static_string("cp-eapi-error-quark");
}
