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

/** Utility macros. */

#ifndef CP_MACROS_H
#define CP_MACROS_H

#include <glib.h>

/*@-exportany@*/

G_BEGIN_DECLS

/*@-namechecks@*/
#ifdef S_SPLINT_S
#   define _(String) String
#else
#   define _(String) g_dgettext(GETTEXT_PACKAGE, (String))
#endif
/*@=namechecks@*/

/*@iter CP_GLIST_ITER(GList *list, yield gpointer elem)@*/

#define CP_GLIST_ITER(list, m_elem) { \
    /*@null@*/ GList *m_iter; \
    for (m_iter = (list); m_iter != NULL; m_iter = m_iter->next) { \
        /*@-incondefs@*/ \
        /*@dependent@*/ /*@null@*/ gpointer m_elem = m_iter->data; \
        /*@=incondefs@*/

#define end_CP_GLIST_ITER }}

G_END_DECLS

#endif
