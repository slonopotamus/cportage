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

#ifndef CP_PATH_H
#define CP_PATH_H

#include <glib.h>

/*@-exportany@*/

/*@-globuse@*/
/**
 * GLib-style realpath() wrapper.
 *
 * \param path  a pathname in the GLib file name encoding
 * \param error return location for a %GError, or %NULL
 * \return      a pathname in the GLib file name encoding
 *              or %NULL if an error occured, free it using g_free()
 */
/*@null@*/ char *
cp_path_realpath(
    const char *path,
    /*@null@*/ GError **error
) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT
/*@modifies *error,errno@*/ /*@globals errno,fileSystem@*/;
/*@=globuse@*/

#endif
