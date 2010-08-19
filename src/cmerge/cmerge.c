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
#include <unistd.h>

#include "config.h"
#include "actions.h"

#define OPTIONS_TABLEEND \
    /*@-nullassign@*/ /*@-type@*/ \
    {NULL, '\0', 0, 0, NULL, NULL, NULL} \
    /*@=type@*/ /*@=nullassign@*/
#define DEFAULT_CONFIG_ROOT "/"

/*@unchecked@*/ static struct actions {
    int clean;
    int depclean;
    int info;
    int install;
    int search;
    int version;
    int help;
} actions;

/*@unchecked@*/ static struct GlobalOptions gopts = {
    VERBOSITY_NORMAL, DEFAULT_CONFIG_ROOT, NULL
};

/*@unchecked@*/ static struct MergeOptions mopts = { &gopts, false, false };

static void
print_version(void)
    /*@globals stdout@*/
    /*@modifies fileSystem,errno,*stdout@*/
{
    g_print("cportage " CP_VERSION "\n\n");
    g_print("Copyright (C) 2009-2010 Marat Radchenko <marat@slonopotamus.org>\n"
         "License GPLv3+: GNU GPL version 3 or later"
             " <http://gnu.org/licenses/gpl.html>\n"
         "This is free software: you are free to change and redistribute it.\n"
         "There is NO WARRANTY, to the extent permitted by law.\n");
}

static bool verbose_cb(
    /*@unused@*/ const char *option_name G_GNUC_UNUSED,
    /*@unused@*/ const char *value G_GNUC_UNUSED,
    /*@unused@*/ void *data G_GNUC_UNUSED,
    /*@unused@*/ GError **error G_GNUC_UNUSED
)
    /*@modifies gopts@*/
{
    gopts.verbosity = VERBOSITY_VERBOSE;
    return true;
}

static bool quiet_cb(
        /*@unused@*/ const char *option_name G_GNUC_UNUSED,
        /*@unused@*/ const char *value G_GNUC_UNUSED,
        /*@unused@*/ void *data G_GNUC_UNUSED,
        /*@unused@*/ GError **error G_GNUC_UNUSED
)
    /*@modifies gopts@*/
{
    gopts.verbosity = VERBOSITY_QUIET;
    return true;
}

/* Know how to compile this without G_GNUC_EXTENSION? Tell me */
/*@unchecked@*/ G_GNUC_EXTENSION static const GOptionEntry options[] = {

    /* Actions */
    {"depclean", 'c', 0, G_OPTION_ARG_NONE, &actions.depclean, NULL, NULL},
    {"help", 'h', 0, G_OPTION_ARG_NONE, &actions.help, NULL, NULL},
    {"info", '\0', 0, G_OPTION_ARG_NONE, &actions.info, NULL, NULL},
    {"install", '\0', 0, G_OPTION_ARG_NONE, &actions.install, NULL, NULL},
    {"search", 's', 0, G_OPTION_ARG_NONE, &actions.search, NULL, NULL},
    {"unmerge", 'C', 0, G_OPTION_ARG_NONE, &actions.clean, NULL, NULL},
    {"version", 'V', 0, G_OPTION_ARG_NONE, &actions.version, NULL, NULL},

    /* Global options */
    {G_OPTION_REMAINING, '\0', 0, G_OPTION_ARG_STRING_ARRAY, &gopts.args, NULL, NULL},
    {"config-root", '\0', 0, G_OPTION_ARG_STRING, &gopts.config_root, NULL, NULL},
    /*@-type@*/
    {"quiet", 'q', G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_CALLBACK, verbose_cb, NULL, NULL},
    {"verbose", 'v', G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_CALLBACK, quiet_cb, NULL, NULL},

    /* Merge options */
    {"pretend", 'p', 0, G_OPTION_ARG_NONE, &mopts.pretend, NULL, NULL},
    {"update", 'u', 0, G_OPTION_ARG_NONE, &mopts.update, NULL, NULL},

    OPTIONS_TABLEEND
};

int
main(int argc, char *argv[])
    /*@modifies actions,errno,*stdout,internalState,fileSystem,argc,argv@*/
{
    GOptionContext *ctx;
    GError *error = NULL;

    (void)setlocale(LC_ALL, "");

    ctx = g_option_context_new(NULL);
    g_option_context_add_main_entries(ctx, options, NULL);
    g_option_context_set_help_enabled(ctx, false);

    if (g_option_context_parse(ctx, &argc, &argv, &error)) {
        int actions_sum = actions.clean
            + actions.depclean
            + actions.help
            + actions.info
            + actions.install
            + actions.search
            + actions.version;

        /*
            Special case for `cmerge foo/bar`.
            We treat it as if --install was specified.
         */
        if (actions_sum == 0 && gopts.args != NULL) {
            actions_sum = actions.install = 1;
        }

        if (actions_sum == 0) {
            char *help = g_option_context_get_help(ctx, false, NULL);
            g_print("%s", help);
            g_free(help);
        } else if (actions_sum > 1) {
            /* TODO: set error */
            g_error("Only one action can be given\n");
        } else if (actions.clean > 0) {
            cmerge_clean_action(&mopts, false, &error);
        } else if (actions.depclean > 0) {
            cmerge_clean_action(&mopts, true, &error);
        } else if (actions.help > 0){
            execlp("man", "man", "cmerge", NULL);
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

    if (error == NULL) {
        return EXIT_SUCCESS;
    } else {
      g_print("%s\n", error->message);
      g_error_free(error);
      return EXIT_FAILURE;
    }
}
