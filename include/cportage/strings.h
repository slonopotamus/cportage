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

/** String functions. */

#ifndef CPORTAGE_STRINGS_H
#define CPORTAGE_STRINGS_H

#include <glib.h>

/*@-exportany@*/

G_BEGIN_DECLS

#pragma GCC visibility push(default)

/*@iter CPORTAGE_STRV_ITER(sef char **arr, yield char *elem)@*/

#define CPORTAGE_STRV_ITER(arr, m_elem) { \
    char **m_iter; \
    for (m_iter = (arr); *m_iter != NULL; ++m_iter) { \
        char *m_elem = *m_iter;
#define end_CPORTAGE_STRV_ITER }}

char **
cportage_strings_pysplit(
    const char *str
) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT /*@*/;

void
cportage_strings_sort(char **str_array) /*@modifies *str_array@*/;

#pragma GCC visibility pop

G_END_DECLS

#endif
