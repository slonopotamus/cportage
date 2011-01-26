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

#include <glib.h>
#include <glib-object.h>
#include <errno.h>
#include <stdlib.h>
#include <cportage/shellconfig.h>

#if HAVE_LOCALE_H
#   include <locale.h>
#endif

#if HAVE_UNISTD_H
#   include <unistd.h>
#elif HAVE_RESOURCE_H
#   include <sys/resource.h>
#endif

#include "actions.h"

typedef int (*CMergeAction) (
    CPSettings settings,
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

/*@observer@*/ static const char *config_root = "/";
/*@observer@*/ static const char *target_root = "/";

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
) /*@modifies action@*/ /*@globals actions,options@*/;

/* Know how to compile this without G_GNUC_EXTENSION? Tell me */
G_GNUC_EXTENSION static const GOptionEntry
options[] = {

    /* Actions */
    {"help", 'h', (gint)G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_CALLBACK, (gpointer)action_cb, NULL, NULL},
    {"info", '\0', (gint)G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_CALLBACK, (gpointer)action_cb, NULL, NULL},
    {"install", '\0', (gint)G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_CALLBACK, (gpointer)action_cb, NULL, NULL},
    {"sync", '\0', (gint)G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_CALLBACK, (gpointer)action_cb, NULL, NULL},
    {"version", 'V', (gint)G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_CALLBACK, (gpointer)action_cb, NULL, NULL},

    /* Global options */
    {G_OPTION_REMAINING, '\0', 0, G_OPTION_ARG_STRING_ARRAY, &opts.args, NULL, NULL},
    {"config-root", '\0', 0, G_OPTION_ARG_STRING, &config_root, NULL, NULL},
    {"target-root", '\0', 0, G_OPTION_ARG_STRING, &target_root, NULL, NULL},
    {"quiet", 'q', (gint)G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_CALLBACK, (gpointer)verbosity_cb, NULL, NULL},
    {"verbose", 'v', (gint)G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_CALLBACK, (gpointer)verbosity_cb, NULL, NULL},

    /* Merge options */
    {"pretend", 'p', 0, G_OPTION_ARG_NONE, &opts.pretend, NULL, NULL},
    {"update", 'u', 0, G_OPTION_ARG_NONE, &opts.update, NULL, NULL},

    /*@-nullassign@*/
    {NULL, '\0', 0, 0, NULL, NULL, NULL}
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

static void
adjust_niceness(
    const CPSettings settings
) /*@modifies *stderr,errno,internalState@*/ /*@globals fileSystem@*/ {
    /*@observer@*/ static const char *key = "PORTAGE_NICENESS";
    const char *value = cp_settings_get(settings, key);
    int inc;

    if (value == NULL) {
        return;
    }
    inc = atoi(value);
    if (inc == 0) {
        return;
    }
    errno = 0;

#if HAVE_NICE
    inc = nice(inc);
#elif HAVE_GETPRIORITY && HAVE_SETPRIORITY
    inc += getpriority(PRIO_PROCESS, 0);
    if (errno) {
        int save_errno = ernno;
        g_warning(_("Can't get current process priority: %s"), g_strerror(save_errno));
        return;
    }
    setpriority(PRIO_PROCESS, 0, inc);
#else
    g_warning(_("%s is specified but system doesn't have neither nice()"
        " nor getpriority()/setpriority() functions"), key);
#endif

    if (errno != 0) {
        int save_errno = errno;
        g_warning(_("Can't change nice value to '%s': %s"),
            value, g_strerror(save_errno));
    }
}

static void
adjust_ionice(
    const CPSettings settings
) /*@modifies *stderr,errno,internalState@*/ /*@globals fileSystem@*/ {
    /*@observer@*/ static const char *key = "PORTAGE_IONICE_COMMAND";
    const char *raw_value = cp_settings_get(settings, key);

    if (raw_value == NULL) {
        return;
    }

#if HAVE_GETPID
    {
#ifndef S_SPLINT_S
        G_STATIC_ASSERT(sizeof(pid_t) <= sizeof(long));
#endif
        GError *error = NULL;
        GHashTable *vars = NULL;
        char *cmd = NULL;
        int retval;
        vars = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
        g_hash_table_insert(vars,
            g_strdup("PID"),
            g_strdup_printf("%ld", (long)getpid()));
        cmd = cp_varexpand(raw_value, vars, &error);
        if (cmd == NULL) {
            goto ERR;
        }
        if (!g_spawn_command_line_sync(cmd, NULL, NULL, &retval, &error)) {
            goto ERR;
        }
        if (retval != EXIT_SUCCESS) {
            g_warning(_("Command '%s' returned %d"), cmd, retval);
		        g_warning(_("See the make.conf(5) man page for %s usage instructions."), key);
        }

ERR:
        if (error != NULL) {
            g_warning(_("Can't run '%s': %s"), cmd, error->message);
            g_error_free(error);
        }
        g_free(cmd);
        if (vars != NULL) {
            g_hash_table_destroy(vars);
        }
    }
#else
    g_warning(_("%s is specified but system doesn't have getpid() function"), key);
#endif
}

int
main(int argc, char *argv[])
/*@modifies *stdout,*stderr,action,errno,internalState,fileSystem@*/
/*@globals opts,options,config_root,target_root,default_action@*/ {
    GOptionContext *ctx;
    GError *error = NULL;
    CPSettings settings = NULL;
    int retval;

#if HAVE_SETLOCALE
    (void)setlocale(LC_ALL, "");
#endif
    g_type_init();

    ctx = g_option_context_new(NULL);
    g_option_context_add_main_entries(ctx, options, NULL);
    g_option_context_set_help_enabled(ctx, FALSE);

    if (!g_option_context_parse(ctx, &argc, &argv, &error)) {
        retval = EXIT_FAILURE;
        goto ERR;
    }

    if (action == NULL) {
        action = default_action;
    }

    settings = cp_settings_new(config_root, target_root, &error);
    if (settings == NULL) {
        retval = EXIT_FAILURE;
        goto ERR;
    }

    adjust_niceness(settings);
    adjust_ionice(settings);

    retval = action->func(settings, &opts, &error);

ERR:
    cp_settings_unref(settings);
    g_option_context_free(ctx);

    if (retval == EXIT_SUCCESS) {
        return retval;
    }

    g_assert(error != NULL);
    g_critical("%s: %s", argv[0], error->message);
    g_error_free(error);

    return retval;
}
