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

#include "atom.h"
#include "collections.h"
#include "eapi.h"
#include "package.h"
#include "settings.h"
#include "strings.h"

struct CPVartree {
    char *path;

    /** Category->packagename->packages cache */
    /*@only@*/ GHashTable *cache;
    gboolean lazy_cache;

    /*@refs@*/ unsigned int refs;
};

static gboolean G_GNUC_WARN_UNUSED_RESULT
try_load_package(
    const CPVartree self,
    const char *category,
    const char *pv,
    /*@out@*/ CPPackage *into,
    /*@null@*/ GError **error
) /*@modifies *into,*error,errno@*/ /*@globals fileSystem@*/ {
    gboolean result = TRUE;
    char *config_file = NULL;

    char *name = NULL;
    CPVersion version = NULL;
    char *slot = NULL;
    char *repo = NULL;

    g_assert(error == NULL || *error == NULL);

    if (!cp_atom_pv_split(pv, &name, &version, NULL)) {
        goto OUT;
    }

    config_file = g_build_filename(self->path, category, pv, "EAPI", NULL);
    result = cp_eapi_check_file(config_file, error);
    g_free(config_file);
    if (!result) {
        goto OUT;
    }

    config_file = g_build_filename(self->path, category, pv, "SLOT", NULL);
    result = g_file_get_contents(config_file, &slot, NULL, error);
    g_free(config_file);
    if (!result) {
        goto OUT;
    }
    slot = g_strstrip(slot);

    result = cp_atom_slot_validate(slot, error);
    if (!result) {
        goto OUT;
    }

    config_file = g_build_filename(self->path, category, pv, "repository", NULL);
    result = g_file_get_contents(config_file, &repo, NULL, error);
    g_free(config_file);
    if (!result) {
        goto OUT;
    }
    repo = g_strstrip(repo);

    result = cp_atom_repo_validate(repo, error);
    if (!result) {
        goto OUT;
    }

    *into = cp_package_new(category, name, version, slot, repo);

OUT:
    g_free(name);
    cp_version_unref(version);
    g_free(slot);
    g_free(repo);

    return result;
}

static void
insert_package(
    GHashTable *name2pkg,
    CPPackage package
) /*@modifies *name2pkg@*/ {
    const char *name = cp_package_name(package);
    /*@only@*/ void *key = NULL;
    GSList *list = NULL;

    if (g_hash_table_lookup_extended(name2pkg, name, &key, (void **)&list)) {
        gboolean stolen = g_hash_table_steal(name2pkg, name);
        g_assert(stolen);
    } else {
        g_assert(key == NULL);
        key = g_strdup(name);
    }

    /*@-refcounttrans@*/
    list = g_slist_prepend(list, package);
    /*@=refcounttrans@*/
    g_hash_table_insert(name2pkg, key, list);
}

static gboolean G_GNUC_WARN_UNUSED_RESULT
populate_cache(
    const CPVartree self,
    const char *category,
    /*@null@*/ GError **error
) /*@modifies *self,*error,errno@*/ /*@globals fileSystem@*/ {
    GHashTable *name2pkg = NULL;
    char *cat_path = NULL;
    GDir *cat_dir = NULL;
    gboolean result = TRUE;

    g_assert(error == NULL || *error == NULL);

    if (!cp_atom_category_validate(category, NULL)) {
        goto OUT;
    }

    cat_path = g_build_filename(self->path, category, NULL);
    if (!g_file_test(cat_path, G_FILE_TEST_IS_DIR)) {
        goto OUT;
    }

    cat_dir = g_dir_open(cat_path, 0, error);
    if (cat_dir == NULL) {
        result = FALSE;
        goto OUT;
    }

    name2pkg = g_hash_table_new_full(
        g_str_hash, g_str_equal, g_free, (GDestroyNotify)cp_package_list_free
    );

    CP_GDIR_ITER(cat_dir, pv) {
        CPPackage package = NULL;

        result = try_load_package(self, category, pv, &package, error);
        if (!result) {
            break;
        }
        if (package != NULL) {
            insert_package(name2pkg, package);
        }
    } end_CP_GDIR_ITER

    g_dir_close(cat_dir);

OUT:
    if (result) {
        g_hash_table_insert(self->cache, g_strdup(category), name2pkg);
    } else {
        cp_hash_table_destroy(name2pkg);
    }
    g_free(cat_path);
    return result;
}

static gboolean G_GNUC_WARN_UNUSED_RESULT
init_cache(
    CPVartree self,
    /*@null@*/ GError **error
) /*@modifies *self,*error,errno@*/ /*@globals fileSystem@*/ {
    GDir *vdb_dir = NULL;
    gboolean result = FALSE;

    g_assert(error == NULL || *error == NULL);

    vdb_dir = g_dir_open(self->path, 0, error);
    if (vdb_dir == NULL) {
        goto ERR;
    }

    CP_GDIR_ITER(vdb_dir, category) {
        if (self->lazy_cache) {
            g_hash_table_insert(self->cache, g_strdup(category), NULL);
            continue;
        }

        if (!populate_cache(self, category, error)) {
            goto ERR;
        }
    } end_CP_GDIR_ITER

    result = TRUE;

ERR:
    g_dir_close(vdb_dir);
    return result;
}

CPVartree
cp_vartree_new(const CPSettings settings, GError **error) {
    CPVartree self;

    g_assert(error == NULL || *error == NULL);

    self = g_new0(struct CPVartree, 1);
    self->refs = (unsigned int)1;
    g_assert(self->path == NULL);
    self->path = g_build_filename(cp_settings_root(settings),
                                  "var", "db", "pkg", NULL);

    self->lazy_cache = cp_string_truth(
        cp_settings_get_default(settings, "CPORTAGE_VARTREE_LAZY", "true")
    ) == CP_TRUE;
    g_assert(self->cache == NULL);
    self->cache = g_hash_table_new_full(
        g_str_hash, g_str_equal, g_free, (GDestroyNotify)cp_hash_table_destroy
    );
    if (!init_cache(self, error)) {
       goto ERR;
    }

    return self;

ERR:
    /*@-usereleased@*/
    cp_vartree_unref(self);
    /*@=usereleased@*/

    return NULL;
}

CPVartree
cp_vartree_ref(CPVartree self) {
    ++self->refs;
    /*@-refcounttrans@*/
    return self;
    /*@=refcounttrans@*/
}

void
cp_vartree_unref(CPVartree self) {
    /*@-mustfreeonly@*/
    if (self == NULL) {
        return;
    }

    g_assert(self->refs > 0);
    if (--self->refs > 0) {
        return;
    }
    /*@=mustfreeonly@*/

    g_free(self->path);
    cp_hash_table_destroy(self->cache);

    /*@-refcounttrans@*/
    g_free(self);
    /*@=refcounttrans@*/
}

static gboolean
get_category_cache(
    CPVartree self,
    const char *cat,
    /*@out@*/ GHashTable **result,
    /*@null@*/ GError **error
) /*@modifies *self,*result,*error,errno@*/ /*@globals fileSystem@*/ {
    g_assert(error == NULL || *error == NULL);

    *result = NULL;

    if (!g_hash_table_lookup_extended(self->cache, cat, NULL, (void **)result)) {
        /* Nonexistent category */
        return TRUE;
    }

    if (*result == NULL) {
        /* Uninited lazy cache */
        g_assert(self->lazy_cache);

        if (!populate_cache(self, cat, error)) {
            return FALSE;
        }

        /*@-dependenttrans@*/
        *result = g_hash_table_lookup(self->cache, cat);
        /*@=dependenttrans@*/
    }

    return TRUE;
}

static gboolean G_GNUC_WARN_UNUSED_RESULT
get_package_cache(
    CPVartree self,
    const char *category,
    const char *package,
    /*@out@*/ GSList **result,
    /*@null@*/ GError **error
) /*@modifies *self,*result,*error,errno@*/ /*@globals fileSystem@*/ {
    GHashTable *name2pkg;

    g_assert(error == NULL || *error == NULL);

    *result = NULL;

    if (!get_category_cache(self, category, &name2pkg, error)) {
        return FALSE;
    }

    if (name2pkg != NULL) {
        /*@-dependenttrans@*/
        *result = g_hash_table_lookup(name2pkg, package);
        /*@=dependenttrans@*/
    }

    return TRUE;
}

gboolean
cp_vartree_find_packages(
    CPVartree self,
    const CPAtom atom,
    GSList **match,
    GError **error
) {
    const char *category = cp_atom_category(atom);
    const char *package = cp_atom_package(atom);
    GSList *pkgs;

    g_assert(error == NULL || *error == NULL);

    *match = NULL;

    if (!get_package_cache(self, category, package, &pkgs, error)) {
        return FALSE;
    }

    CP_GSLIST_ITER(pkgs, pkg) {
        if (cp_atom_matches(atom, pkg)) {
            /*
              TODO: instead of sorting here, keep cache reverse-sorted
              and use g_slist_prepend.
             */
            /*@-mustfreefresh@*/
            *match = g_slist_insert_sorted(
                *match,
                cp_package_ref(pkg),
                (GCompareFunc)cp_package_cmp);
            /*@=mustfreefresh@*/
        }
    } end_CP_GSLIST_ITER

    return TRUE;
}

const char *
cp_vartree_path(const CPVartree self) {
    return self->path;
}
