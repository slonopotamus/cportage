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

/** String functions. */

#ifndef CP_STRINGS_H
#define CP_STRINGS_H

#include <cportage/macros.h>

/*@-exportany@*/

G_BEGIN_DECLS

#pragma GCC visibility push(default)

/*@iter CP_STRV_ITER(sef char **arr, yield char *elem)@*/

#define CP_STRV_ITER(arr, m_elem) CP_ITER(char *, arr, m_elem)
#define end_CP_STRV_ITER end_CP_ITER

/**
 * Splits given string at whitespace. Empty elements are filtered out.
 *
 * \param str a string to split
 * \return a %NULL-terminated string array, free it using g_strfreev()
 */
char **
cp_strings_pysplit(
    const char *str
) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT /*@*/;

/**
 * Sorts %NULL-terminated string array in place.
 *
 * \param str_array array to sort.
 */
void
cp_strings_sort(char **str_array) /*@modifies *str_array@*/;

/**
 * Parses string as boolean
 *
 * \return %FALSE if \a str is one of "false", "f", "no", "n", "0",
 *         %TRUE otherwise
 */
gboolean
cp_string_is_true(/*@null@*/ const char *str) G_GNUC_WARN_UNUSED_RESULT /*@*/;

#pragma GCC visibility pop

G_END_DECLS

#endif
