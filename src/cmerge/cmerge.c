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
/* TODO: this is only included for puts(). Have some glib alternative? */
#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "actions.h"

#define OPTIONS_TABLEEND {NULL, '\0', 0, 0, NULL, NULL, NULL}
#define DEFAULT_CONFIG_ROOT "/"

static struct {
    int clean;
    int depclean;
    int info;
    int install;
    int search;
    int version;
} actions;

static struct GlobalOptions gopts = { VERBOSITY_NORMAL, DEFAULT_CONFIG_ROOT, NULL };

static struct MergeOptions mopts = { &gopts, 0, 0 };

static void
print_version(void) {
    g_print("cportage " CPORTAGE_VERSION "\n\n");
    g_print("Copyright (C) 2009-2010 Marat Radchenko <marat@slonopotamus.org>\n"
         "License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>\n"
         "This is free software: you are free to change and redistribute it.\n"
         "There is NO WARRANTY, to the extent permitted by law.\n");
}

static bool verbose_cb(
        const char *option_name G_GNUC_UNUSED,
        const char *value G_GNUC_UNUSED,
        void *data G_GNUC_UNUSED,
        GError **error G_GNUC_UNUSED) {
    gopts.verbosity = VERBOSITY_VERBOSE;
    return true;
}

static bool quiet_cb(
        const char *option_name G_GNUC_UNUSED,
        const char *value G_GNUC_UNUSED,
        void *data G_GNUC_UNUSED,
        GError **error G_GNUC_UNUSED) {
    gopts.verbosity = VERBOSITY_QUIET;
    return true;
}

static const GOptionEntry actions_options[] = {
    {"depclean", 'c', 0, G_OPTION_ARG_NONE, &actions.depclean,
        "Clean the system by removing packages that are not associated"
        " with explicitly merged packages", NULL},
    {"info", '\0', 0, G_OPTION_ARG_NONE, &actions.info,
        "Produce a list of information to include in bug reports", NULL},
    {"install", '\0', 0, G_OPTION_ARG_NONE, &actions.install,
        "Install packages (default action)", NULL},
    {"search", 's', 0, G_OPTION_ARG_NONE, &actions.search,
        "Search for matches of the supplied string in the portage tree", NULL},
    {"unmerge", 'C', 0, G_OPTION_ARG_NONE, &actions.clean,
        "Remove all matching packages", NULL},
    {"version", 'V', 0, G_OPTION_ARG_NONE, &actions.version,
        "Output version", NULL},
    OPTIONS_TABLEEND
};

/* Know how to compile this without extension? Tell me */
G_GNUC_EXTENSION static const GOptionEntry gopts_options[] = {
    {G_OPTION_REMAINING, '\0', 0, G_OPTION_ARG_STRING_ARRAY, &gopts.args,
        "Leftover args", NULL},
    {"config-root", '\0', 0, G_OPTION_ARG_STRING, &gopts.config_root,
        "Set location for configuration files", "DIR"},
    {"quiet", 'q', G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_CALLBACK, verbose_cb,
        "Enable quiet output mode", NULL},
    {"verbose", 'v', G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_CALLBACK, quiet_cb,
        "Enable verbose output mode", NULL},
    OPTIONS_TABLEEND
};

static const GOptionEntry mopts_options[] = {
    {"pretend", 'p', 0, G_OPTION_ARG_NONE, &mopts.pretend,
        "Instead of actually performing any action, only display what would be done", NULL},
    {"update", 'u', 0, G_OPTION_ARG_NONE, &mopts.update,
        "Update packages to the best version available", NULL},
    OPTIONS_TABLEEND
};

/*static const GOptionEntry popts[] = {
    {NULL, 0, POPT_ARG_INCLUDE_TABLE, &actions_options, 0,
        "Actions (only one can be specified)", NULL},
    {NULL, 0, POPT_ARG_INCLUDE_TABLE, &gopts_options, 0, "General Options", NULL},
    {NULL, 0, POPT_ARG_INCLUDE_TABLE, &mopts_options, 0, "Merge Options"
        " (taken into account when requested action performes"
        " package (un)merging)", NULL},
    OPTIONS_TABLEEND
};*/

static GOptionContext *
create_option_ctx(void) {
    GOptionContext *ctx = g_option_context_new("[ACTION] [ARGS...]");

    GOptionGroup *actions_group = g_option_group_new("actions",
        "Actions (only one can be specified):",
        "Show available actions to perform", NULL, NULL);
    GOptionGroup *gopts_group = g_option_group_new("global",
        "Global options (applicable to all actions):",
        "Show global options", NULL, NULL);
    GOptionGroup *mopts_group = g_option_group_new("merge",
        "Merge/unmerge options:",
        "Show merge/unmerge options", NULL, NULL);

    g_option_group_add_entries(actions_group, actions_options);
    g_option_context_add_group(ctx, actions_group);
    g_option_group_add_entries(gopts_group, gopts_options);
    g_option_context_add_group(ctx, gopts_group);
    g_option_group_add_entries(mopts_group, mopts_options);
    g_option_context_add_group(ctx, mopts_group);

    return ctx;
}

int
main(int argc, char *argv[]) {
    GOptionContext *ctx;
    GError *error = NULL;

    (void)setlocale(LC_ALL, "");

    ctx = create_option_ctx();

    if (g_option_context_parse(ctx, &argc, &argv, &error)) {
        int actions_sum = actions.clean + actions.depclean + actions.info
            + actions.install + actions.search + actions.version;

        /*
            Special case for `cmerge foo/bar`.
            We treat it as if --install was specified.
         */
        if (actions_sum == 0 && gopts.args != NULL) {
            actions_sum = actions.install = 1;
        }

        if (actions_sum == 0) {
            char *help = g_option_context_get_help(ctx, false, NULL);
            puts(help);
            g_free(help);
        } else if (actions_sum > 1) {
            /* TODO: set error */
            g_error("Only one action can be given\n");
        } else if (actions.clean > 0) {
            cmerge_clean_action(&mopts, FALSE, &error);
        } else if (actions.depclean > 0) {
            cmerge_clean_action(&mopts, TRUE, &error);
        } else if (actions.info > 0) {
            cmerge_info_action(&gopts, &error);
        } else if (actions.install > 0) {
           cmerge_install_action(&mopts, &error);
        } else if (actions.search > 0) {
            cmerge_search_action(&gopts, &error);
        } else if (actions.version > 0) {
            print_version();
        } else {
            /* TODO: set error */
            g_error("Unknown action");
        }
    }
    g_option_context_free(ctx);

    if (error == NULL)
        return EXIT_SUCCESS;
    else {
      g_print("%s\n", error->message);
      g_error_free(error);
      return EXIT_FAILURE;
    }
}
