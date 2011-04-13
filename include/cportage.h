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

#ifndef CPORTAGE_H
#define CPORTAGE_H

#include <glib.h>
#include <stdio.h>

G_BEGIN_DECLS

#pragma GCC visibility push(default)

/*@-exportany@*/
/*@-exportiter@*/

/**
 * Sorts %NULL-terminated string array in place.
 *
 * \param str_array array to sort.
 */
void
cp_strings_sort(char **str_array) /*@modifies *str_array@*/;

/*@null@*/ /*@only@*/ char *
cp_io_get_relative_path(
    const char *parent,
    const char *descendant
) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT /*@*/;

/**
 * GLib-style fopen() wrapper.
 *
 * \param path  a pathname in the GLib file name encoding
 * \param error return location for a %GError, or %NULL
 * \param mode  open mode (see fopen() for possible values)
 * \return      a %FILE pointer or %NULL if an error occured
 */
/*@dependent@*/ /*@null@*/ FILE *
cp_io_fopen(
    const char *path,
    const char *mode,
    /*@null@*/ GError **error
) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT
/*@modifies *error,errno,fileSystem@*/ /*@globals fileSystem@*/;

/**
 * Reads a single line of input (including line separator if it was encountered).
 *
 * \param file      a %FILE pointer
 * \param file_desc human-readable description of stream (filename, url, etc)
 * \param into      return location for read line, free it using g_free().
 *                  Only modified on successful read.
 * \param error     return location for a %GError, or %NULL
 * \return          negative number on error, 0 if EOF was reached,
 *                  positive number on successful read
 */
int
cp_io_getline(
    FILE *file,
    const char *file_desc,
    /*@out@*/ char **into,
    /*@null@*/ GError **error
) G_GNUC_WARN_UNUSED_RESULT
/*@modifies *file,*into,*error,errno@*/ /*@globals fileSystem@*/;

/**
 * Fully reads text file and splits it at line endings. Skips empty lines.
 *
 * \param path            a pathname in the GLib file name encoding
 * \param ignore_comments if %TRUE, comments starting with \c '#' will be
 *                        excluded
 * \param error           return location for a %GError, or %NULL
 * \return                a %NULL-terminated string array
 *                        or %NULL if an error occurred,
 *                        free it using g_strfreev()
 */
/*@null@*/ /*@only@*/ char **
cp_io_getlines(
    const char *path,
    const gboolean ignore_comments,
    /*@null@*/ GError **error
) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT
/*@modifies *error,errno@*/ /*@globals fileSystem@*/;

/*@-declundef@*/
/** TODO: documentation */
/*@null@*/ char *
cp_varexpand(
    const char *str,
    GHashTable *vars,
    /*@null@*/ GError **error
) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT /*@modifies *error@*/;
/*@=declundef@*/

/**
 * A structure describing a single repository.
 */
typedef /*@refcounted@*/ struct CPRepository *CPRepository;

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

/*@iter CP_REPOSITORY_ITER(CPRepository *arr, yield CPRepository elem)@*/

#define CP_REPOSITORY_ITER(arr, m_elem) { \
    CPRepository *m_iter; \
    for (m_iter = (arr); *m_iter != NULL; ++m_iter) { \
        CPRepository m_elem = *m_iter;

#define end_CP_REPOSITORY_ITER }}

/**
 * Central immutable storage of cportage configuration.
 */
typedef /*@refcounted@*/ struct CPSettings *CPSettings;

/**
 * Reads global and profile configuration data
 * and stores it in a #CPSettings immutable structure.
 *
 * \param root  path to configuration root dir (typically "/")
 * \param error return location for a %GError, or %NULL
 * \return      a #CPSettings structure, free it using cp_settings_unref()
 */
/*@newref@*/ /*@null@*/ CPSettings
cp_settings_new(
    const char *root,
    /*@null@*/ GError **error
) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT
/*@modifies *error,*stderr,errno@*/ /*@globals fileSystem@*/;

/**
 * Increases reference count of \a self by 1.
 *
 * \param self a #CPSettings structure
 * \return \a self
 */
/*@newref@*/ CPSettings
cp_settings_ref(CPSettings self) G_GNUC_WARN_UNUSED_RESULT /*@modifies *self@*/;

/**
 * Decreases reference count of \a self by 1. When reference count drops
 * to zero, it frees all the memory associated with the structure.
 *
 * \param self a #CPSettings
 */
void
cp_settings_unref(/*@killref@*/ /*@null@*/ CPSettings self) /*@modifies self@*/;

/**
 * \return readonly canonical path to profile directory
 */
/*@observer@*/ const char *
cp_settings_profile(const CPSettings self) G_GNUC_WARN_UNUSED_RESULT /*@*/;

/**
 * \return 'main' repository in \a self
 */
/*@newref@*/ CPRepository
cp_settings_main_repository(
    const CPSettings self
) G_GNUC_WARN_UNUSED_RESULT /*@modifies self@*/;

/**
 * \return readonly NULL-terminated list of repositories (including main repo)
 *         in \a self, ordered by their priority, ascending
 */
/*@observer@*/ CPRepository *
cp_settings_repositories(
    const CPSettings self
) G_GNUC_WARN_UNUSED_RESULT /*@*/;

/**
 * \return readonly value of \a key variable
 *         or \a fallback if variable is not set
 */
/*@observer@*/ const char *
cp_settings_get_default(
    const CPSettings self,
    const char *key,
    const char *fallback
) G_GNUC_WARN_UNUSED_RESULT /*@*/;

/**
 * \param error return location for a %GError, or %NULL
 * \return readonly value of \a key variable
 *         or %NULL if variable is not set
 */
/*@null@*/ /*@observer@*/ const char *
cp_settings_get_required(
    const CPSettings self,
    const char *key,
    /*@null@*/ GError **error
) G_GNUC_WARN_UNUSED_RESULT /*@modifies *error@*/;

/**
 * \return readonly value of \a key variable
 *         or %NULL if variable is not set
 */
/*@null@*/ /*@observer@*/ const char *
cp_settings_get(
    const CPSettings self,
    const char *key
) G_GNUC_WARN_UNUSED_RESULT /*@*/;

/**
 * Structure, describing single package version.
 */
typedef /*@refcounted@*/ struct CPVersion *CPVersion;

/**
 * Creates new #CPVersion structure for \a version.
 *
 * \param version a version string
 * \param error   return location for a %GError, or %NULL
 * \return        a #CPVersion structure, free it using cp_version_unref()
 */
/*@newref@*/ /*@null@*/ CPVersion
cp_version_new(
    const char *version,
    /*@null@*/ GError **error
) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT /*@modifies *error@*/;

/**
 * Increases reference count of \a self by 1.
 *
 * \param self a #CPVersion structure
 * \return \a self
 */
/*@newref@*/ CPVersion
cp_version_ref(CPVersion self) G_GNUC_WARN_UNUSED_RESULT /*@modifies *self@*/;

/**
 * Decreases reference count of \a self by 1. When reference count drops
 * to zero, it frees all the memory associated with the structure.
 *
 * \param self a #CPVersion
 */
void
cp_version_unref(/*@killref@*/ /*@null@*/ CPVersion self) /*@modifies self@*/;

/**
 * Comparator for #CPVersion.
 */
int
cp_version_cmp(
    const CPVersion first,
    const CPVersion second
) G_GNUC_WARN_UNUSED_RESULT /*@*/;

/**
 * \return readonly string representation of \a self
 */
/*@observer@*/ const char *
cp_version_str(const CPVersion self) G_GNUC_WARN_UNUSED_RESULT /*@*/;

/**
 * Structure, describing a single package.
 */
typedef /*@refcounted@*/ struct CPPackage *CPPackage;

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
 * Frees \a list of #CPackage instances.
 */
void
cp_package_list_free(/*@null@*/ /*@only@*/ GList *list) /*@modifies list@*/;

/**
 * Comparator for #CPPackage.
 */
int
cp_package_cmp(
    const CPPackage first,
    const CPPackage second
) G_GNUC_WARN_UNUSED_RESULT /*@*/;

/**
 * \return readonly category name of \a self
 */
/*@observer@*/ const char *
cp_package_category(const CPPackage self) G_GNUC_WARN_UNUSED_RESULT /*@*/;

/**
 * \return readonly package name of \a self
 */
/*@observer@*/ const char *
cp_package_name(const CPPackage self) G_GNUC_WARN_UNUSED_RESULT /*@*/;

/**
 * \return version of \a self. Free it using cp_version_unref().
 */
/*@newref@*/ CPVersion
cp_package_version(
    const CPPackage self
) G_GNUC_WARN_UNUSED_RESULT /*@modifies self@*/;

/**
 * \return readonly slot name of \a self
 */
/*@observer@*/ const char *
cp_package_slot(const CPPackage self) G_GNUC_WARN_UNUSED_RESULT /*@*/;

/**
 * \return readonly repository name of \a self
 */
/*@observer@*/ const char *
cp_package_repo(const CPPackage self) G_GNUC_WARN_UNUSED_RESULT /*@*/;

/**
 * Structure, describing a single atom.
 */
typedef /*@refcounted@*/ struct CPAtom *CPAtom;

/**
 * Creates a #CPAtom structure for \a value.
 *
 * \param error return location for a %GError, or %NULL
 * \return      a #CPAtom, free it using cp_atom_unref()
 */
/*@newref@*/ /*@null@*/ CPAtom
cp_atom_new(
    const char *value,
    /*@null@*/ GError **error
) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT
/*@modifies *error@*/;

/**
 * Increases reference count of \a self by 1.
 *
 * \param self a #CPAtom
 * \return \a self
 */
/*@newref@*/ CPAtom
cp_atom_ref(CPAtom self) G_GNUC_WARN_UNUSED_RESULT /*@modifies *self@*/;

/**
 * Decreases reference count of \a self by 1. When reference count drops
 * to zero, it frees all the memory associated with the structure.
 *
 * \param self a #CPAtom
 */
void
cp_atom_unref(/*@killref@*/ /*@null@*/ CPAtom self) /*@modifies self@*/;

/**
 * \return %TRUE if \a self matches \a package
 */
gboolean
cp_atom_matches(
    const CPAtom self,
    const CPPackage package
) G_GNUC_WARN_UNUSED_RESULT /*@modifies package@*/;

/**
 * Installed packages tree.
 */
typedef /*@refcounted@*/ struct CPVartree *CPVartree;

/**
 * TODO: documentation.
 */
/*@newref@*/ CPVartree
cp_vartree_new(
    const CPSettings settings
) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT /*@*/;

/**
 * Increases reference count of \a self by 1.
 *
 * \param self a #CPVartree structure
 * \return \a self
 */
/*@newref@*/ CPVartree
cp_vartree_ref(CPVartree self) G_GNUC_WARN_UNUSED_RESULT /*@modifies *self@*/;

/**
 * Decreases reference count of \a self by 1. When reference count drops
 * to zero, it frees all the memory associated with the structure.
 *
 * \param self a #CPVartree
 */
void
cp_vartree_unref(/*@killref@*/ /*@null@*/ CPVartree self) /*@modifies self@*/;

/**
 * TODO: documentation.
 */
gboolean
cp_vartree_find_packages(
    CPVartree self,
    const CPAtom atom,
    /*@out@*/ GList/*<CPPackage>*/ **match,
    /*@null@*/ GError **error
) G_GNUC_WARN_UNUSED_RESULT
/*@modifies self,*match,*error@*/ /*@globals fileSystem@*/;

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
        /*@dependent@*/ /*@null@*/ void *m_elem = m_iter->data; \
        /*@=incondefs@*/

#define end_CP_GLIST_ITER }}

/*@iter CP_STRV_ITER(char **arr, yield char *elem)@*/

#define CP_STRV_ITER(arr, m_elem) { \
    char **m_iter; \
    for (m_iter = (arr); *m_iter != NULL; ++m_iter) { \
        char *m_elem = *m_iter;

#define end_CP_STRV_ITER }}

#pragma GCC visibility pop

G_END_DECLS

#endif
