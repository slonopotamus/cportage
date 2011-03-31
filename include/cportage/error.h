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

#if !defined(CPORTAGE_H_INSIDE) && !defined(CPORTAGE_COMPILATION)
#error "Only <cportage.h> can be included directly."
#endif

#ifndef CP_ERROR_H
#define CP_ERROR_H

#include <glib.h>

/*@-exportany@*/

G_BEGIN_DECLS

#pragma GCC visibility push(default)

#define CP_ERROR cp_error_quark()

GQuark
cp_error_quark(void) /*@*/;

/**
 * Error codes for cportage library.
 */
enum CPError {
    CP_ERROR_EAPI_UNSUPPORTED,
    CP_ERROR_ATOM_SYNTAX,
    /*@-enummemuse@*/
    CP_ERROR_SHELLCONFIG_SOURCE_DISABLED,
    CP_ERROR_SHELLCONFIG_SYNTAX,
    /*@=enummemuse@*/
    CP_ERROR_SETTINGS_REQUIRED_MISSING
};

#pragma GCC visibility pop

G_END_DECLS

#endif
