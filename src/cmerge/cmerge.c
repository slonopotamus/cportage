/*
    Copyright 2009-2010, Marat Radchenko

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

#include <locale.h>
#include <stdlib.h>

#include "actions.h"

static gboolean
action_cb(const char *option_name, const char *value, void *data, GError **error);

typedef struct ActionDesc {
    CMergeAction func;
} ActionDesc;

static ActionDesc actions[] = {
    { cmerge_help_action },
    { cmerge_info_action },
    { cmerge_install_action },
    { cmerge_sync_action },
    { cmerge_version_action },
};

static ActionDesc *action = NULL;
static const char *config_root = "/";
static const char *target_root = "/";

/*@unchecked@*/ static struct CMergeOptions
opts = { NULL, VERBOSITY_NORMAL, FALSE, FALSE };

static gboolean
verbosity_cb(
    const char *option_name,
    /*@unused@*/ const char *value G_GNUC_UNUSED,
    /*@unused@*/ void *data G_GNUC_UNUSED,
    /*@unused@*/ GError **error G_GNUC_UNUSED
) /*@modifies opts@*/ {
    const gboolean verbose = g_strcmp0(option_name, "--verbose") == 0
        || g_strcmp0(option_name, "-v");
    opts.verbosity= verbose ? VERBOSITY_VERBOSE : VERBOSITY_QUIET;
    return TRUE;
}

/* Know how to compile this without G_GNUC_EXTENSION? Tell me */
/*@unchecked@*/ G_GNUC_EXTENSION static const GOptionEntry
options[] = {

    /* Actions */
    {"help", 'h', G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_CALLBACK, action_cb, NULL, NULL},
    {"info", '\0', G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_CALLBACK, action_cb, NULL, NULL},
    {"install", '\0', G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_CALLBACK, action_cb, NULL, NULL},
    {"sync", '\0', G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_CALLBACK, action_cb, NULL, NULL},
    {"version", 'V', G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_CALLBACK, action_cb, NULL, NULL},

    /* Global options */
    {G_OPTION_REMAINING, '\0', 0, G_OPTION_ARG_STRING_ARRAY, &opts.args, NULL, NULL},
    {"config-root", '\0', 0, G_OPTION_ARG_STRING, &config_root, NULL, NULL},
    {"target-root", '\0', 0, G_OPTION_ARG_STRING, &target_root, NULL, NULL},
    /*@-type@*/
    {"quiet", 'q', G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_CALLBACK, verbosity_cb, NULL, NULL},
    {"verbose", 'v', G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_CALLBACK, verbosity_cb, NULL, NULL},

    /* Merge options */
    {"pretend", 'p', 0, G_OPTION_ARG_NONE, &opts.pretend, NULL, NULL},
    {"update", 'u', 0, G_OPTION_ARG_NONE, &opts.update, NULL, NULL},

    /*@-nullassign@*/ /*@-type@*/
    {NULL, '\0', 0, 0, NULL, NULL, NULL}
    /*@=type@*/ /*@=nullassign@*/
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
main(int argc, char *argv[]) {
    GOptionContext *ctx;
    GError *error = NULL;
    CPSettings settings = NULL;

    (void)setlocale(LC_ALL, "");

    ctx = g_option_context_new(NULL);
    g_option_context_add_main_entries(ctx, options, NULL);
    g_option_context_set_help_enabled(ctx, FALSE);

    if (!g_option_context_parse(ctx, &argc, &argv, &error)) {
        goto ERR;
    }

    /*
        Special case for `cmerge foo/bar`.
        We treat it as if --install was specified.
    */
    if (action == NULL) {
        action = &actions[2];
    }

    settings = cp_settings_new(config_root, target_root, &error);
    if (settings == NULL) {
        goto ERR;
    }

    action->func(settings, &opts, &error);

ERR:
    cp_settings_unref(settings);
    g_option_context_free(ctx);

    if (error == NULL) {
        return EXIT_SUCCESS;
    } else {
        g_print("%s: %s\n", argv[0], error->message);
        g_error_free(error);
        return EXIT_FAILURE;
    }
}
