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

#include "config.h"
#include "cmerge/actions.h"

static void
print_version(void) {
    g_print("cportage " CPORTAGE_VERSION "\n\n");
    g_print("Copyright (C) 2009-2010 Marat Radchenko <marat@slonopotamus.org>\n"
         "License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>\n"
         "This is free software: you are free to change and redistribute it.\n"
         "There is NO WARRANTY, to the extent permitted by law.\n");
}

int
main(int argc, char *argv[]) {
    int clean = 0,
        help = 0,
        info = 0,
        install = 0,
        search = 0,
        version = 0;
    /*const struct poptOption actions[] = {
        // TODO: alias this to --clean, -c, --prune, -P, --unmerge
        {"depclean", 'C', POPT_ARG_NONE, &clean, 0, "Cleans the system"
            " by removing packages that are not associated"
            " with explicitly merged packages", NULL},
        {"help", 'h', POPT_ARG_NONE, &help, 0, "Shows this help message", NULL},
        {"info", 0, POPT_ARG_NONE, &info, 0, "Produces a list of information"
         " to include in bug reports", NULL},
        {"install", 0, POPT_ARG_NONE, &install, 0, "Installs package", NULL},
        {"search", 's', POPT_ARG_NONE, &search, 0, "Searches for matches"
         " of the supplied string in the portage tree.", NULL},
        {"version", 'V', POPT_ARG_NONE, &version, 0, "Outputs version", NULL},
        POPT_TABLEEND
    };
    */
    struct MergeOptions mopts = { { VERBOSITY_NORMAL, "/", NULL }, 0, 0 };
    /*
    const struct poptOption moptions[] = {
        {"pretend", 'p', POPT_ARG_NONE, &mopts.pretend, 0, "Instead of actually"
            " performing any action, only displays what would be done", NULL},
        {"update", 'u', POPT_ARG_NONE, &mopts.update, 0, "Updates packages"
         " to the best version available", NULL},
        POPT_TABLEEND
    };
    const struct poptOption popts[] = {
        {NULL, 0, POPT_ARG_INCLUDE_TABLE, &actions, 0,
            "Actions (only one can be specified)", NULL},
        {NULL, 0, POPT_ARG_INCLUDE_TABLE, &goptions, 0, "General Options", NULL},
        {NULL, 0, POPT_ARG_INCLUDE_TABLE, &moptions, 0, "Merge Options"
         " (taken into account when requested action performes"
         " package (un)merging)", NULL},
        POPT_TABLEEND
    };
    */
    const GOptionEntry entries[] = {
        /*{"config-root", 0, POPT_ARG_STRING | POPT_ARGFLAG_SHOW_DEFAULT,
            &gopts.config_root, 0, "Specifies the location"
            " for configuration files", "DIR"},
        {"quiet", 'q', POPT_ARG_VAL, &gopts.verbosity, VERBOSITY_QUIET,
         "Enables quiet output mode", NULL},
        {"verbose", 'v', POPT_ARG_VAL, &gopts.verbosity, VERBOSITY_VERBOSE,
         "Enables verbose output mode", NULL},
         */
        { NULL, 0, 0, 0, NULL, NULL, NULL }
    };
    GOptionContext *ctx;
    GError *error = NULL;

    setlocale(LC_ALL, "");

    ctx = g_option_context_new("ACTION [ARGS...]");
    g_option_context_add_main_entries(ctx, entries, NULL);

    if (g_option_context_parse(ctx, &argc, &argv, &error)) {
        int actions = clean + help + info + install + search + version;

        /*
            Special case for `cmerge foo/bar`.
            We treat it as if --install was specified.
         */
        if (actions == 0 && mopts.global.args != NULL)
            actions = install = 1;

        if (actions == 0 || help)
            (void)0;
            /* poptPrintHelp(ctx, stdout, 0); */
        else if (actions > 1)
            g_error("Only one action can be given\n");
        else if (version)
            print_version();
        else if (clean)
            cmerge_clean_action(&mopts, &error);
        else if (info)
            cmerge_info_action(&mopts.global, &error);
        else if (install)
           cmerge_install_action(&mopts, &error);
        else if (search)
            cmerge_search_action(&mopts.global, &error);
        else
            g_error("Unknown action");
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
