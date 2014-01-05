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
#include "strings.h"

const char *
cp_eapi_str(CPEapi eapi) {
    switch (eapi) {
        case CP_EAPI_0:
            return "0";

        case CP_EAPI_1:
            return "1";

        case CP_EAPI_2:
            return "2";

        case CP_EAPI_3:
            return "3";

        case CP_EAPI_4:
            return "4";

        case CP_EAPI_5:
            return "5";

        case CP_EAPI_UNKNOWN:
        default:
            return "unknown";
    }
}

CPEapi
cp_eapi_parse(const char *eapi, const char *file, GError **error) {
    g_assert(error == NULL || *error == NULL);

    if (eapi[0] == '\0' || eapi[1] != '\0') {
        goto ERR;
    }

    switch (eapi[0]) {
        case '0':
            return CP_EAPI_0;

        case '1':
            return CP_EAPI_1;

        case '2':
            return CP_EAPI_2;

        case '3':
            return CP_EAPI_3;

        case '4':
            return CP_EAPI_4;

        case '5':
            return CP_EAPI_5;

        default:
            goto ERR;
    }

ERR:
    g_set_error(error, CP_ERROR, (gint)CP_ERROR_EAPI_UNSUPPORTED,
        _("Unsupported EAPI '%s' in '%s'"), eapi, file);
    return CP_EAPI_UNKNOWN;
}

CPEapi
cp_eapi_parse_file(const char *file, GError **error) {
    char *data;
    CPEapi result;

    g_assert(error == NULL || *error == NULL);

    if (!g_file_test(file, G_FILE_TEST_EXISTS)) {
        return CP_EAPI_LATEST;
    }

    if (!g_file_get_contents(file, &data, NULL, error)) {
        return CP_EAPI_UNKNOWN;
    }

    result = cp_eapi_parse(g_strstrip(data), file, error);

    g_free(data);
    return result;
}

gboolean
cp_eapi_check(CPEapi eapi, GError **error) {
    g_assert(error == NULL || *error == NULL);

    if (eapi == CP_EAPI_UNKNOWN) {
        g_set_error(error, CP_ERROR, (gint)CP_ERROR_EAPI_UNSUPPORTED,
            _("Unsupported EAPI given"));
        return FALSE;
    }

    return TRUE;
}

gboolean
cp_eapi_has_slot_deps(CPEapi eapi) {
    return eapi >= CP_EAPI_1;
}

gboolean
cp_eapi_has_use_deps(CPEapi eapi) {
    return eapi >= CP_EAPI_2;
}

gboolean
cp_eapi_has_strong_blocks(CPEapi eapi) {
    return eapi >= CP_EAPI_2;
}

gboolean
cp_eapi_has_subslots(CPEapi eapi) {
    return eapi >= CP_EAPI_5;
}
