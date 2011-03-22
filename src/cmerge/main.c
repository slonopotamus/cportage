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

#include "config.h"

#include <glib-object.h>
#include <errno.h>
#include <stdlib.h>

#if HAVE_LOCALE_H
#   include <locale.h>
#endif

#include "actions.h"
#include "nice.h"

typedef int (*CMergeAction) (
    CPContext ctx,
    const CMergeOptions options,
    /*@null@*/ GError **error
);

typedef const struct ActionDesc {
    CMergeAction func;
} *ActionDesc;

static const struct ActionDesc actions[] = {
    { cmerge_help_action },
    { cmerge_info_action },
    { cmerge_install_action },
    { cmerge_sync_action },
    { cmerge_version_action },
};
static const ActionDesc default_action = &actions[2];
/*@null@*/ static ActionDesc action = NULL;

typedef enum {
    VERBOSITY_QUIET = -1,
    VERBOSITY_NORMAL = 0,
    VERBOSITY_VERBOSE = 1
} VerbosityLevel;

static VerbosityLevel verbosity = VERBOSITY_NORMAL;

/*@observer@*/ static const char *root = "/";

static struct CMergeOptions opts = { NULL, FALSE, FALSE };

static gboolean
verbosity_cb(
    const char *option_name,
    /*@unused@*/ const char *value G_GNUC_UNUSED,
    /*@unused@*/ void *data G_GNUC_UNUSED,
    /*@unused@*/ GError **error G_GNUC_UNUSED
) /*@modifies verbosity@*/ {
    const gboolean verbose = g_strcmp0(option_name, "--verbose") == 0
        || g_strcmp0(option_name, "-v") == 0;
    verbosity = verbose ? VERBOSITY_VERBOSE : VERBOSITY_QUIET;
    return TRUE;
}

static gboolean
action_cb(
    const char *option_name,
    const char *value,
    void *data,
    GError **error
) /*@modifies action@*/ /*@globals actions@*/;

/* Know how to compile this without G_GNUC_EXTENSION? Tell me */
/*@unchecked@*/ G_GNUC_EXTENSION static const GOptionEntry
options[] = {

    /* Actions */
    {"help", 'h', (gint)G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_CALLBACK, (gpointer)action_cb, NULL, NULL},
    {"info", '\0', (gint)G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_CALLBACK, (gpointer)action_cb, NULL, NULL},
    {"install", '\0', (gint)G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_CALLBACK, (gpointer)action_cb, NULL, NULL},
    {"sync", '\0', (gint)G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_CALLBACK, (gpointer)action_cb, NULL, NULL},
    {"version", 'V', (gint)G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_CALLBACK, (gpointer)action_cb, NULL, NULL},

    /* Global options */
    {G_OPTION_REMAINING, '\0', 0, G_OPTION_ARG_STRING_ARRAY, &opts.args, NULL, NULL},
    {"config-root", '\0', 0, G_OPTION_ARG_STRING, &root, NULL, NULL},
    {"quiet", 'q', (gint)G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_CALLBACK, (gpointer)verbosity_cb, NULL, NULL},
    {"verbose", 'v', (gint)G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_CALLBACK, (gpointer)verbosity_cb, NULL, NULL},

    /* Merge options */
    {"pretend", 'p', 0, G_OPTION_ARG_NONE, &opts.pretend, NULL, NULL},
    {"update", 'u', 0, G_OPTION_ARG_NONE, &opts.update, NULL, NULL},

    /*@-nullassign@*/
    {NULL, '\0', 0, (GOptionArg)0, NULL, NULL, NULL}
    /*@=nullassign@*/
};

static gboolean
action_cb(
    const char *option_name,
    /*@unused@*/ const char *value G_GNUC_UNUSED,
    /*@unused@*/ void *data G_GNUC_UNUSED,
    /*@unused@*/ GError **error G_GNUC_UNUSED
) {
    size_t i;

    if (action != NULL) {
        /* TODO: set error. More than one action given. */
        return FALSE;
    }

    for (i = 0; i < G_N_ELEMENTS(actions); ++i) {
        const GOptionEntry entry = options[i];
        char *long_name = g_strdup_printf("--%s", entry.long_name);
        char *short_name = entry.short_name == '\0'
            ? NULL
            : g_strdup_printf("-%c", entry.short_name);
        if (g_strcmp0(option_name, long_name) == 0
                || g_strcmp0(option_name, short_name) == 0) {
            action = &actions[i];
        }
        g_free(long_name);
        g_free(short_name);
        if (action != NULL) {
            return TRUE;
        }
    }

    /* TODO: set error */
    return FALSE;
}

int
main(int argc, char *argv[])
/*@modifies argc,argv,*stdout,*stderr,action,errno,internalState,fileSystem@*/
/*@globals opts,root,default_action@*/ {
    GOptionContext *opt_ctx;
    GError *error = NULL;
    struct CPContext ctx = { NULL, NULL };
    int retval;

#if HAVE_SETLOCALE
    (void)setlocale(LC_ALL, "");
#endif
    g_type_init();

    opt_ctx = g_option_context_new(NULL);
    g_option_context_add_main_entries(opt_ctx, options, NULL);
    g_option_context_set_help_enabled(opt_ctx, FALSE);

    if (!g_option_context_parse(opt_ctx, &argc, &argv, &error)) {
        retval = EXIT_FAILURE;
        goto ERR;
    }

    if (action == NULL) {
        action = default_action;
    }

    ctx.settings = cp_settings_new(root, &error);
    if (ctx.settings == NULL) {
        retval = EXIT_FAILURE;
        goto ERR;
    }

    adjust_niceness(ctx.settings);
    adjust_ionice(ctx.settings);

    ctx.vartree = cp_vartree_new(ctx.settings);

    retval = action->func(&ctx, &opts, &error);

ERR:
    cp_vartree_unref(ctx.vartree);
    cp_settings_unref(ctx.settings);
    g_option_context_free(opt_ctx);

    if (retval == EXIT_SUCCESS) {
        return retval;
    }

    g_assert(error != NULL);
    g_critical("%s", error->message);
    g_error_free(error);

    return retval;
}
