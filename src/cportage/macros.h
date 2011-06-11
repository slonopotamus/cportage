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

/** Utility macros. */

#ifndef CP_MACROS_H
#define CP_MACROS_H

#include <glib.h>

/*@iter CP_GDIR_ITER(GDir *dir, yield const char *elem)@*/

#define CP_GDIR_ITER(dir, m_elem) { \
    /*@-incondefs@*/ \
    /*@dependent@*/ /*@observer@*/ const char *m_elem; \
    /*@=incondefs@*/ \
    /*@-modnomods@*/ \
    while ((m_elem = g_dir_read_name(dir)) != NULL) { \
    /*@=modnomods@*/

#define end_CP_GDIR_ITER }}

/*@-namechecks@*/
#ifdef S_SPLINT_S
#   define _(String) String
#else
#   define _(String) g_dgettext(GETTEXT_PACKAGE, (String))
#endif
/*@=namechecks@*/

#endif
