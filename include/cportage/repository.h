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

#ifndef CP_REPOSITORY_H
#define CP_REPOSITORY_H

#include <cportage/macros.h>

/*@-exportany@*/

G_BEGIN_DECLS

#pragma GCC visibility push(default)

/**
 * A structure describing a single repository.
 */
typedef /*@refcounted@*/ struct CPRepository *CPRepository;

/*@iter CP_REPOSITORY_ITER(CPRepository *arr, yield CPRepository elem)@*/

#define CP_REPOSITORY_ITER(arr, m_elem) { \
    CPRepository *m_iter; \
    for (m_iter = (arr); *m_iter != NULL; ++m_iter) { \
        CPRepository m_elem = *m_iter;

#define end_CP_REPOSITORY_ITER }}

/**
 * Creates a #CPRepository with given path. Assumes path is absolute.
 *
 * \return a #CPRepository, free it using cp_repository_unref()
 */
/*@newref@*/ CPRepository
cp_repository_new(
    const char *path
) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT
/*@modifies *stderr,errno@*/ /*@globals fileSystem@*/;

/**
 * Increases reference count of \a self by 1.
 *
 * \param self a #CPRepository structure
 * \return \a self
 */
/*@newref@*/ CPRepository
cp_repository_ref(
    CPRepository self
) G_GNUC_WARN_UNUSED_RESULT /*@modifies *self@*/;

/**
 * Decreases reference count of \a self by 1. When reference count drops
 * to zero, it frees all the memory associated with the structure.
 *
 * \param self a #CPRepository
 */
void
cp_repository_unref(
    /*@killref@*/ /*@null@*/ CPRepository self
) /*@modifies self@*/;

/**
 * \return readonly canonical path to \a self root
 */
/*@observer@*/ const char *
cp_repository_path(const CPRepository self) G_GNUC_WARN_UNUSED_RESULT /*@*/;

/**
 * \return readonly \a self name
 */
/*@observer@*/ const char *
cp_repository_name(const CPRepository self) G_GNUC_WARN_UNUSED_RESULT /*@*/;

/** TODO: documentation */
int
cp_repository_sync(
    const CPRepository self,
    /*@null@*/ GError **error
) G_GNUC_WARN_UNUSED_RESULT
/*@modifies *error,*stdout,*stderr,errno,fileSystem@*/;

#pragma GCC visibility pop

G_END_DECLS

#endif
