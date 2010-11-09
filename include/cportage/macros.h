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

#if !defined(CPORTAGE_H_INSIDE) && !defined(CPORTAGE_COMPILATION)
#error "Only <cportage.h> can be included directly."
#endif

/** Utility macros. */

#ifndef CP_MACROS_H
#define CP_MACROS_H

#include <glib.h>

/*@-exportany@*/

G_BEGIN_DECLS

#define _(String) g_dgettext(GETTEXT_PACKAGE, (String))

#define CP_ITER(type, arr, m_elem) { \
    type *m_iter = (arr); \
    g_assert(m_iter != NULL); \
    while (*m_iter != NULL) { \
        type m_elem = *m_iter++;
#define end_CP_ITER }}

G_END_DECLS

#endif
