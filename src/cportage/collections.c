/*
    Copyright 2009-2014, Marat Radchenko

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

#include <string.h>
#include <cportage.h>

#include "collections.h"

struct removal_data {
    /*@null@*/ GSList *to_remove;
    /*@shared@*/ GHRFunc func;
    /*@null@*/ /*@temp@*/ void *user_data;
};

static gboolean
collect_removals(/*@keep@*/ void *key, void *value, void *user_data) {
    struct removal_data *data = user_data;

    /*@-branchstate@*/
    if (data->func(key, value, data->user_data)) {
        data->to_remove = g_slist_prepend(data->to_remove, key);
    }
    /*@=branchstate@*/

    return FALSE;
}

void
cp_tree_foreach_remove(GTree *tree, GHRFunc func, void *user_data) {
    struct removal_data data;

    data.to_remove = NULL;
    data.func = func;
    data.user_data = user_data;

    g_tree_foreach(tree, collect_removals, &data);

    CP_GSLIST_ITER(data.to_remove, elem) {
        (void)g_tree_remove(tree, elem);
    } end_CP_GSLIST_ITER

    g_slist_free(data.to_remove);
}

struct copy_data {
    GTree *dest;
    CPCopyFunc key_copy_func;
    CPCopyFunc value_copy_func;
    /*@null@*/ void *user_data;
};

static gboolean
copy_entry(void *key, void *value, void *user_data) {
    struct copy_data *data = user_data;

    g_tree_insert(
        data->dest,
        data->key_copy_func(key, data->user_data),
        data->value_copy_func(value, data->user_data)
    );

    return TRUE;
}

void
cp_tree_insert_all(
    GTree *src,
    GTree *dest,
    CPCopyFunc key_copy_func,
    CPCopyFunc value_copy_func,
    void *user_data
) {
    struct copy_data data;

    data.dest = dest;
    data.key_copy_func = key_copy_func;
    data.value_copy_func = value_copy_func;
    data.user_data = user_data;

    g_tree_foreach(src, copy_entry, &data);
}

void
cp_hash_table_destroy(GHashTable *hash_table) {
    if (hash_table != NULL) {
        g_hash_table_destroy(hash_table);
    }
}

void
cp_tree_destroy(GTree *tree) {
    if (tree != NULL) {
        g_tree_destroy(tree);
    }
}

gboolean
cp_true_filter(
    /*@unused@*/ void *key G_GNUC_UNUSED,
    /*@unused@*/ void *value G_GNUC_UNUSED,
    /*@unused@*/ void *user_data G_GNUC_UNUSED
) /*@*/ {
    return TRUE;
}

void
cp_stack_dict(GTree *tree, char **items) /*@modifies *tree@*/ {
    CP_STRV_ITER(items, item) {
        if (strcmp(item, "-*") == 0) {
            cp_tree_foreach_remove(tree, cp_true_filter, NULL);
        } else if (item[0] == '-') {
            (void)g_tree_remove(tree, &item[1]);
        } else {
            g_tree_insert(tree, g_strdup(item), NULL);
        }
    } end_CP_STRV_ITER
}
