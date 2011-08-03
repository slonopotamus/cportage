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

void
cp_hash_table_destroy(
    /*@null@*/ /*@only@*/ GHashTable *hash_table
) /*@modifies hash_table@*/;

void
cp_tree_destroy(
    /*@null@*/ /*@only@*/ GTree *tree
) /*@modifies tree@*/;

/**
 * \return relative part of \a descendant to \a parent or %NULL if \a descendant
 *         isn't a descendant of \a parent, free it using g_free()
 */
/*@null@*/ /*@only@*/ char *
cp_path_get_relative(
    const char *parent,
    const char *descendant
) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT /*@*/;

gboolean
cp_path_is_descendant(
    const char *parent,
    const char *descendant
) G_GNUC_WARN_UNUSED_RESULT /*@*/;

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

/**
 * Replaces ${..} placeholders in \a str with values from \a vars.
 *
 * \param error return location for a %GError, or %NULL
 * \return      a string with replaced placeholders
 *              or %NULL if an error occurred, free it using g_free()
 */
/*@null@*/ char *
cp_varexpand(
    const char *str,
    GHashTable/*<char *, char *>*/ *vars,
    /*@null@*/ GError **error
) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT /*@modifies *error@*/;

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

/**
 * Synchronizes repository contents from remote location (rsync or VCS).
 * Synchronization method depends on repository settings.
 *
 * ATTENTION: after returning from this function, all #CPSettings instances
 * that own repository with same path as \a self become invalid and need to be
 * recreated (that also applies to all objects that were created from now-invalid
 * #CPSettings objects, transitively.
 *
 * \param error return location for a %GError, or %NULL
 * \return      %EXIT_SUCCESS on success, other value if an error occurred.
 */
int
cp_repository_sync(
    const CPRepository self,
    /*@null@*/ GError **error
) G_GNUC_WARN_UNUSED_RESULT
/*@modifies *error,*stdout,*stderr,errno,fileSystem@*/;

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
 * \return readonly list of repositories (including main repo)
 *         in \a self, ordered by their priority, ascending
 */
/*@observer@*/ GSList * /*<CPRepository>*/
cp_settings_repositories(
    const CPSettings self
) G_GNUC_WARN_UNUSED_RESULT /*@*/;

/*@null@*/ /*@newref@*/ CPRepository
cp_settings_get_repository(
    const CPSettings self,
    const char *name
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
cp_package_list_free(/*@null@*/ /*@only@*/ GSList *list) /*@modifies list@*/;

/**
 * Comparator for #CPPackage.
 */
int
cp_package_cmp(
    const CPPackage first,
    const CPPackage second
) G_GNUC_WARN_UNUSED_RESULT /*@*/;

/**
 * \return readonly string representation of \a self
 */
/*@observer@*/ const char *
cp_package_str(const CPPackage self) G_GNUC_WARN_UNUSED_RESULT /*@*/;

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
 * \return %TRUE if \a self matches \a package, %FALSE otherwise
 */
gboolean
cp_atom_matches(
    const CPAtom self,
    const CPPackage package
) G_GNUC_WARN_UNUSED_RESULT /*@modifies *package@*/;

typedef struct CPConfigProtect *CPConfigProtect;

CPConfigProtect
cp_config_protect_new(
    CPSettings settings
) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT /*@*/;

void
cp_config_protect_destroy(
    /*@null@*/ /*@only@*/ CPConfigProtect self
) /*@modifies self@*/;

gboolean
cp_config_protect_is_protected(
    const CPConfigProtect self,
    const char *path
) G_GNUC_WARN_UNUSED_RESULT /*@*/;

/**
 * Installed packages tree.
 */
typedef /*@refcounted@*/ struct CPVartree *CPVartree;

/**
 * TODO: documentation.
 */
/*@newref@*/ /*@null@*/ CPVartree
cp_vartree_new(
    const CPSettings settings,
    /*@null@*/ GError **error
) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT
/*@modifies *error,errno*/ /*@globals fileSystem@*/;

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

/*@observer@*/ const char *
cp_vartree_path(const CPVartree self) G_GNUC_WARN_UNUSED_RESULT /*@*/;

/**
 * Searches for packages matching \a atom in \a self.
 *
 * \param atom  atom to match against
 * \param match return location for matched atoms list,
 *              free it using cp_package_list_free()
 * \param error return location for a %GError, or %NULL
 * \return      %TRUE on success, %FALSE if an error occurred
 *
 * \see cp_atom_matches()
 */
gboolean
cp_vartree_find_packages(
    CPVartree self,
    const CPAtom atom,
    /*@out@*/ GSList/*<CPPackage>*/ **match,
    /*@null@*/ GError **error
) G_GNUC_WARN_UNUSED_RESULT
/*@modifies self,*match,*error@*/ /*@globals fileSystem@*/;

/*
  This macro intentionally conflicts with GLib one. The only difference is that
  we don't do cast to 'char *'.
 */
/*@-namechecks@*/
#ifdef S_SPLINT_S
#   define _(String) String
#else
#   define _(String) g_dgettext(GETTEXT_PACKAGE, (String))
#endif
/*@=namechecks@*/

/*
 * Iterators intentionally do not support nesting to:
 * 1. Workaround Splint unawareness of ## preprocessor operator
 * 2. To avoid overcomplicated nested loops
 */

/*@iter CP_GSLIST_ITER(GSList *list, yield gpointer elem)@*/

#define CP_GSLIST_ITER(list, m_elem) { \
    /*@null@*/ GSList *m_iter = (list); \
    for (; m_iter != NULL; m_iter = m_iter->next) { \
        /*@-incondefs@*/ \
        /*@dependent@*/ /*@null@*/ void *m_elem = m_iter->data; \
        /*@=incondefs@*/

#define end_CP_GSLIST_ITER }}

/*@iter CP_STRV_ITER(char **arr, yield char *elem)@*/

#define CP_STRV_ITER(arr, m_elem) { \
    char **m_iter = (arr); \
    for (; *m_iter != NULL; ++m_iter) { \
        char *m_elem = *m_iter;

#define end_CP_STRV_ITER }}

/*@iter CP_GDIR_ITER(GDir *dir, yield const char *elem)@*/

#define CP_GDIR_ITER(dir, m_elem) { \
    /*@-incondefs@*/ \
    /*@dependent@*/ /*@observer@*/ const char *m_elem; \
    /*@=incondefs@*/ \
    /*@-modnomods@*/ \
    while ((m_elem = g_dir_read_name(dir)) != NULL) { \
    /*@=modnomods@*/

#define end_CP_GDIR_ITER }}

#pragma GCC visibility pop

G_END_DECLS

#endif
