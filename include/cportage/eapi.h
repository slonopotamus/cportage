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

#ifndef CP_EAPI_H
#define CP_EAPI_H

#include <glib.h>

/*@-exportany@*/

G_BEGIN_DECLS

#pragma GCC visibility push(default)

#define CP_EAPI_ERROR cp_eapi_error_quark()

GQuark
cp_eapi_error_quark(void);

typedef enum {
    CP_EAPI_ERROR_UNSUPPORTED
} CPEAPIError;

gboolean
cp_eapi_check(
    const char *eapi,
    const char *file,
    GError **error
) /*@modifies *error@*/ G_GNUC_WARN_UNUSED_RESULT;

#pragma GCC visibility pop

G_END_DECLS

#endif
