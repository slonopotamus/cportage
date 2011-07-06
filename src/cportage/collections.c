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

#include <string.h>
#include <cportage.h>

#include "collections.h"

static void
do_remove(void *elem, void *user_data) {
    g_tree_remove(user_data, elem);
}

struct removal_data {
    /*@null@*/ GSList *to_remove;
    GHRFunc func;
    /*@null@*/ void *user_data;
};

static gboolean
collect_removals(void *key, void *value, void *user_data) {
    struct removal_data *data = user_data;

    if (data->func(key, value, data->user_data)) {
        data->to_remove = g_slist_append(data->to_remove, key);
    }

    return FALSE;
}

void
cp_tree_foreach_remove(GTree *tree, GHRFunc func, void *user_data) {
    struct removal_data data;

    data.to_remove = NULL;
    data.func = func;
    data.user_data = user_data;

    g_tree_foreach(tree, collect_removals, &data);
    g_slist_foreach(data.to_remove, do_remove, tree);
    g_slist_free(data.to_remove);
}

void
cp_hash_table_destroy(GHashTable *hash_table) {
    if (hash_table) {
        g_hash_table_destroy(hash_table);
    }
}

gboolean
cp_true_filter(
    void *key G_GNUC_UNUSED,
    void *value G_GNUC_UNUSED,
    void *user_data G_GNUC_UNUSED
) /*@*/ {
    return TRUE;
}

void
cp_stack_dict(GTree *tree, char **items) /*@modifies *tree@*/ {
    CP_STRV_ITER(items, item) {
        if (strcmp(item, "-*") == 0) {
            cp_tree_foreach_remove(tree, cp_true_filter, NULL);
        } else if (item[0] == '-') {
            g_tree_remove(tree, &item[1]);
        } else {
            g_tree_insert(tree, g_strdup(item), NULL);
        }
    } end_CP_STRV_ITER
}
