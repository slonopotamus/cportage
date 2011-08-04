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

#include "collections.h"
#include "incrementals.h"
#include "strings.h"

struct CPIncrementals {
    /*@dependent@*/ GTree/*<char *, char *>*/ *config;

    /*@only@*/ GTree/*<char *, GTree<char *, NULL>>*/ *incrementals;
    /*@only@*/ GTree/*<char *, NULL>*/ *use_mask;
    /*@only@*/ GTree/*<char *, NULL>*/ *use_force;
};

/*@observer@*/ /*@unchecked@*/ static const char * const
default_incrementals[] = {
    "USE", "USE_EXPAND", "USE_EXPAND_HIDDEN", "FEATURES", "ACCEPT_KEYWORDS",
    "CONFIG_PROTECT_MASK", "CONFIG_PROTECT", "PRELINK_PATH",
    "PRELINK_PATH_MASK", "PROFILE_ONLY_VARIABLES"
};

static /*@dependent@*/ GTree *
register_incremental(CPIncrementals self, const char *key) /*@modifies *self@*/ {
    GTree *result = g_tree_lookup(self->incrementals, key);

    if (result == NULL) {
        result = g_tree_new_full((GCompareDataFunc)strcmp, NULL, g_free, NULL);
        g_tree_insert(self->incrementals, g_strdup(key), result);
    }

    return result;
}

static void
add_incremental(
    CPIncrementals self,
    const char *key,
    /*@null@*/ const char *value,
    gboolean stack_use_expand
) /*@modifies *self@*/ {
    char **items;
    GTree *values;

    if (value == NULL) {
        return;
    }

    items = cp_strings_pysplit(value);
    if (items == NULL) {
        return;
    }

    values = register_incremental(self, key);

    /*
      As a long-standing practice, USE_EXPAND variables are only stacked across
      profiles, but not in make.conf. So, in order to be compatible with portage
      (and official Gentoo docs), we have this logic here.
      Yes, it is PMS violation.
     */
    if (!stack_use_expand) {
        GTree *use_expand = g_tree_lookup(self->incrementals, "USE_EXPAND");
        if (use_expand != NULL
                && g_tree_lookup_extended(use_expand, key, NULL, NULL)) {
            cp_tree_foreach_remove(values, cp_true_filter, NULL);
        }
    }

    cp_stack_dict(values, items);

    g_strfreev(items);
}

struct add_incrementals_data {
    /*@dependent@*/ CPIncrementals self;
    gboolean stack_use_expand;
};

static gboolean
add_incrementals(
    void *key,
    /*@unused@*/ void *value G_GNUC_UNUSED,
    void *user_data
) /*@*/ {
    struct add_incrementals_data *data = user_data;
    CPIncrementals self = data->self;

    add_incremental(
        self, key, g_tree_lookup(self->config, key), data->stack_use_expand
    );

    /* See post_process_incremental comments */
    (void)g_tree_remove(self->config, key);

    return FALSE;
}

static gboolean
force_use(
    void *key,
    /*@unused@*/ void *value G_GNUC_UNUSED,
    void *user_data
) /*@modifies *user_data@*/ {
    g_tree_insert(user_data, g_strdup(key), NULL);

    return FALSE;
}

static gboolean
remove_masked_use(
    void *key,
    /*@unused@*/ void *value G_GNUC_UNUSED,
    void *user_data
) /*@modifies *user_data@*/ {
    (void)g_tree_remove(user_data, key);

    return FALSE;
}

struct populate_use_data {
    /*@dependent@*/ CPIncrementals self;
    char *prefix;
};

static gboolean
populate_from_use_expand(
    void *key,
    /*@unused@*/ void *value G_GNUC_UNUSED,
    void *user_data
) /*@*/ {
    struct populate_use_data *data = user_data;
    char *str = g_strdup_printf("%s_%s", data->prefix, (char *)key);

    add_incremental(data->self, "USE", str, TRUE);

    g_free(str);
    return FALSE;
}

static gboolean
populate_from_use_expands(
    void *key,
    /*@unused@*/ void *value G_GNUC_UNUSED,
    void *user_data
) /*@*/ {
    CPIncrementals self = user_data;
    struct populate_use_data data;
    GTree *values = g_tree_lookup(self->incrementals, key);

    if (values == NULL) {
        goto OUT;
    }

    data.self = self;
    data.prefix = g_ascii_strdown(key, (ssize_t)-1);

    g_tree_foreach(values, populate_from_use_expand, &data);

    g_free(data.prefix);

OUT:
    return FALSE;
}

static gboolean
concat_key(
    void *key,
    /*@unused@*/ void *value G_GNUC_UNUSED,
    void *user_data
) /*@modifies *user_data@*/ {
    GString *str = user_data;
    if (str->len > 0) {
        (void)g_string_append_c(str, ' ');
    }

    (void)g_string_append(str, key);

    return FALSE;
}

static /*@only@*/ char * G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT
concat_keys(GTree *tree) /*@*/ {
    char *result;
    GString *str = g_string_new("");
    g_tree_foreach(tree, concat_key, str);
    result = g_string_free(str, FALSE);
    g_assert(result != NULL);
    return result;
}

static gboolean
add_to_tree(
    /*@keep@*/ void *key,
    /*@keep@*/ void *value,
    void *user_data
) /*@modifies *user_data@*/ {
    g_tree_insert(user_data, key, value);

    return FALSE;
}

struct use_expand_item_data {
    char *prefix;
    GTree *values;
};

static gboolean
filter_use_expand(
    void *key,
    /*@unused@*/ void *value G_GNUC_UNUSED,
    void *user_data
) /*@*/ {
    struct use_expand_item_data *data = user_data;
    const char *str_key = key;

    if (!g_str_has_prefix(str_key, data->prefix)) {
        return FALSE;
    }

    g_tree_insert(data->values, g_strdup(&str_key[strlen(data->prefix)]), NULL);

    return TRUE;
}

struct use_expand_data {
    /*@dependent@*/ CPIncrementals self;
    /*@dependent@*/ GTree *use_no_expand;
};

static gboolean
regenerate_use_expand(
    void *key,
    /*@unused@*/ void *value G_GNUC_UNUSED,
    void *user_data
) /*@*/ {
    struct use_expand_data *data = user_data;
    struct use_expand_item_data item_data;
    char *lower_key = g_ascii_strdown(key, (ssize_t)-1);

    item_data.prefix = g_strdup_printf("%s_", lower_key);
    item_data.values = g_tree_new_full(
        (GCompareDataFunc)strcmp, NULL, g_free, NULL
    );

    cp_tree_foreach_remove(data->use_no_expand, filter_use_expand, &item_data);

    g_free(lower_key);
    g_free(item_data.prefix);

    g_tree_insert(
        data->self->config, g_strdup(key), concat_keys(item_data.values)
    );

    g_tree_destroy(item_data.values);
    return FALSE;
}

static gboolean
post_process_incremental(
    void *key,
    void *value,
    void *user_data
) /*@modifies *value,*user_data@*/ {
    CPIncrementals self = user_data;

    if (strcmp(key, "USE") == 0) {
        GTree *use_no_expand = g_tree_new((GCompareFunc)strcmp);
        struct use_expand_data data;
        GTree *use_expand = g_tree_lookup(self->incrementals, "USE_EXPAND");

        /* PMS, section 12.1.1 */
        add_incremental(self, "USE", g_tree_lookup(self->config, "ARCH"), TRUE);

        /* PMS, section 12.1.1 */
        if (use_expand != NULL) {
            g_tree_foreach(use_expand, populate_from_use_expands, self);
        }

        data.self = self;
        data.use_no_expand = use_no_expand;

        /* PMS, section 5.2.12 */
        g_tree_foreach(self->use_force, force_use, value);
        g_tree_foreach(self->use_mask, remove_masked_use, value);

        g_tree_foreach(value, add_to_tree, use_no_expand);

        /*
          We push values back from USE to USE_EXPAND variables. Otherwise,
          USE_EXPAND variables would be unsafe to use because use.{force,mask}
          handling ignores them. PMS doesn't say anything on this though.
          Portage has this logic too.
         */
        if (use_expand != NULL) {
            g_tree_foreach(use_expand, regenerate_use_expand, &data);
        }

        g_tree_insert(
            self->config,
            g_strdup("CPORTAGE_USE_NO_EXPAND"),
            concat_keys(use_no_expand)
        );

        g_tree_destroy(use_no_expand);
    }

    /*
     * Since PMS doesn't say when incremental stacking and use.{mask,force}
     * handling should happen and what values variables should have until it
     * happened, we do handling after all config files were loaded (once).
     * We also clear incremental variables in self->config after each file
     * was parsed (in add_incrementals) so that we don't parse same string
     * over and over again for all config files after the one it appeared in.
     */
    g_tree_insert(self->config, g_strdup(key), concat_keys(value));

    return FALSE;
}

static gboolean G_GNUC_WARN_UNUSED_RESULT
collect_profile_list(
    const char *profile_dir,
    const char *file,
    GTree *into,
    /*@null@*/ GError **error
) /*@modifies *into,*error,errno@*/ /*@globals fileSystem@*/ {
    char *config_file;
    gboolean result = FALSE;

    g_assert(error == NULL || *error == NULL);

    config_file = g_build_filename(profile_dir, file, NULL);

    if (g_file_test(config_file, G_FILE_TEST_EXISTS)) {
        char **lines = cp_io_getlines(config_file, TRUE, error);
        if (lines == NULL) {
            goto ERR;
        }
        cp_stack_dict(into, lines);
        g_strfreev(lines);
    }

    result = TRUE;

ERR:
    g_free(config_file);
    return result;
}

CPIncrementals
cp_incrementals_new(GTree *config) {
    CPIncrementals self;
    size_t i;

    self = g_new0(struct CPIncrementals, 1);

    g_assert(self->config == NULL);
    self->config = config;

    g_assert(self->incrementals == NULL);
    self->incrementals = g_tree_new_full(
        (GCompareDataFunc)strcmp, NULL, g_free, (GDestroyNotify)g_tree_destroy
    );
    for (i = 0; i < G_N_ELEMENTS(default_incrementals); ++i) {
        (void)register_incremental(self, default_incrementals[i]);
    }

    g_assert(self->use_mask == NULL);
    self->use_mask = g_tree_new_full(
        (GCompareDataFunc)strcmp, NULL, g_free, NULL
    );

    g_assert(self->use_force == NULL);
    self->use_force = g_tree_new_full(
        (GCompareDataFunc)strcmp, NULL, g_free, NULL
    );

    return self;
}

void
cp_incrementals_destroy(CPIncrementals self) {
    if (self == NULL) {
        /*@-mustfreeonly@*/
        return;
        /*@=mustfreeonly@*/
    }

    cp_tree_destroy(self->incrementals);
    cp_tree_destroy(self->use_mask);
    cp_tree_destroy(self->use_force);

    /*@-refcounttrans@*/
    g_free(self);
    /*@=refcounttrans@*/
}

gboolean
cp_incrementals_process_profile(
    CPIncrementals self,
    const char *dir,
    GError **error
) {
    g_assert(error == NULL || *error == NULL);

    if (!collect_profile_list(dir, "use.mask", self->use_mask, error)) {
        return FALSE;
    }

    if (!collect_profile_list(dir, "use.force", self->use_force, error)) {
        return FALSE;
    }

    return TRUE;
}

void
cp_incrementals_config_changed(CPIncrementals self, gboolean stack_use_expand) {
    GTree *use_expand;
    struct add_incrementals_data data;

    data.self = self;
    data.stack_use_expand = stack_use_expand;

    g_tree_foreach(self->incrementals, add_incrementals, &data);

    /* PMS, section 5.3.2 */
    /*
      Spec is kinda broken.
      1. USE_EXPAND is incremental
      2. Values of USE_EXPAND variables go to _incremental_ USE.

      Suppose we have a parent profile with USE_EXPAND="FOO" FOO="bar" and
      child profile with USE_EXPAND="-FOO". Now tell me, what resulting USE
      is supposed to be? Currently, we don't unregister variables that once
      became incremental and will have USE="foo_bar". Another confusing example
      is USE_EXPAND="FOO" FOO="bar" USE_EXPAND="-FOO" in a single file.
      Need to talk to Zac.
     */
    use_expand = g_tree_lookup(self->incrementals, "USE_EXPAND");
    if (use_expand != NULL) {
        g_tree_foreach(use_expand, add_incrementals, &data);
    }
}

void
cp_incrementals_config_finished(CPIncrementals self) {
    g_tree_foreach(self->incrementals, post_process_incremental, self);
}

gboolean
cp_incrementals_contains(
    CPIncrementals self,
    const char *key,
    const char *value
) {
    GTree *values = g_tree_lookup(self->incrementals, key);
    if (values == NULL) {
        return FALSE;
    }

    return g_tree_lookup_extended(values, value, NULL, NULL);
}
