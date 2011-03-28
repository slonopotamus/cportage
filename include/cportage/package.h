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

#ifndef CP_PACKAGE_H
#define CP_PACKAGE_H

#include <cportage/version.h>

/*@-exportany@*/

G_BEGIN_DECLS

#pragma GCC visibility push(default)

/**
 * TODO: documentation.
 */
typedef /*@refcounted@*/ struct CPPackage *CPPackage;

/**
 * TODO: documentation.
 */
/*@newref@*/ CPPackage
cp_package_new(
    const char *category,
    const char *name,
    CPVersion version,
    const char *slot,
    const char *repo
) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT /*@modifies version@*/;

/**
 * Increases reference count of \a self by 1.
 *
 * \param self a #CPPackage structure
 * \return \a self
 */
/*@newref@*/ CPPackage
cp_package_ref(CPPackage self) G_GNUC_WARN_UNUSED_RESULT /*@modifies *self@*/;

/**
 * Decreases reference count of \a self by 1. When reference count drops
 * to zero, it frees all the memory associated with the structure.
 *
 * \param self a #CPPackage
 */
void
cp_package_unref(/*@killref@*/ /*@null@*/ CPPackage self) /*@modifies self@*/;

/**
 * TODO: documentation.
 */
void
cp_package_list_free(/*@null@*/ /*@only@*/ GList *list) /*@modifies list@*/;

/**
 * TODO: documentation.
 */
/*@observer@*/ const char *
cp_package_category(const CPPackage self) G_GNUC_WARN_UNUSED_RESULT /*@*/;

/**
 * TODO: documentation.
 */
/*@observer@*/ const char *
cp_package_name(const CPPackage self) G_GNUC_WARN_UNUSED_RESULT /*@*/;

/**
 * TODO: documentation.
 */
/*@newref@*/ CPVersion
cp_package_version(
    const CPPackage self
) G_GNUC_WARN_UNUSED_RESULT /*@modifies self@*/;

/**
 * TODO: documentation.
 */
/*@observer@*/ const char *
cp_package_slot(const CPPackage self) G_GNUC_WARN_UNUSED_RESULT /*@*/;

/**
 * TODO: documentation.
 */
/*@observer@*/ const char *
cp_package_repo(const CPPackage self) G_GNUC_WARN_UNUSED_RESULT /*@*/;

/**
 * TODO: documentation.
 */
int
cp_package_cmp(
    const CPPackage first,
    const CPPackage second
) G_GNUC_WARN_UNUSED_RESULT /*@*/;

#pragma GCC visibility pop

G_END_DECLS

#endif
