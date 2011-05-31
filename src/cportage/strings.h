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

/** String functions. */

#ifndef CP_STRINGS_H
#define CP_STRINGS_H

#include <cportage.h>

/*@-exportany@*/

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
 * Parses string as boolean
 *
 * \return %FALSE if \a str is one of "false", "f", "no", "n", "0",
 *         %TRUE otherwise
 */
gboolean
cp_string_is_true(
    /*@null@*/ const char *str
) G_GNUC_WARN_UNUSED_RESULT
/*@modifies *stderr,errno@*/ /*@globals internalState@*/;

#endif
