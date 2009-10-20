/*
	Copyright 2009, Marat Radchenko

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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <popt.h>

#include "config.h"
#include "cportage/atom.h"
#include "cportage/settings.h"

void print_version(void) {
	printf("cportage %s\n\n", CPORTAGE_VERSION);
	puts("Copyright (C) 2009 Marat Radchenko <marat@slonopotamus.org>\n"
		"License GPLv3+: GNU GPL version 3 or later"
		" <http://gnu.org/licenses/gpl.html>\n"
		"This is free software: you are free to change and redistribute it.\n"
		"There is NO WARRANTY, to the extent permitted by law.");
}

int main(const int argc, const char * argv[]) {
	int depclean = 0, help = 0, info = 0, install = 0, search = 0, update = 0,
		version = 0;
	const struct poptOption actions[] = {
		#warning TODO: alias this to --clean, -c, --prune, -P, --unmerge
		{"depclean", 'C', POPT_ARG_NONE, &depclean, 0, "Cleans the system"
			" by removing packages that are not associated"
			" with explicitly merged packages", NULL},
		{"help", 'h', POPT_ARG_NONE, &help, 0, "Shows this help message", NULL},
		{"info", 0, POPT_ARG_NONE, &info, 0, "Produces a list of information"
			" to include in bug reports", NULL},
		{"install", 0, POPT_ARG_NONE, &install, 0, "Installs package", NULL},
		{"search", 's', POPT_ARG_NONE, &search, 0, "Searches for matches"
			" of the supplied string in the portage tree.", NULL},
		{"update", 'u', POPT_ARG_NONE, &update, 0, "Updates packages"
			" to the best version available", NULL},
		{"version", 'V', POPT_ARG_NONE, &version, 0, "Outputs version", NULL},
		POPT_TABLEEND
	};
	char * config_root = "/";
	int verbose = 0, pretend = 0;
	const struct poptOption goptions[] = {
		{"config-root", 0, POPT_ARG_STRING | POPT_ARGFLAG_SHOW_DEFAULT,
			&config_root, 0, "Specifies the location for configuration files",
			"DIR"},
		{"verbose", 'v', POPT_ARG_NONE, &verbose, 0,
			"Tells cmerge to run in verbose mode", NULL},
		POPT_TABLEEND
	};
	const struct poptOption moptions[] = {
		{"pretend", 'p', POPT_ARG_NONE, &pretend, 0, "Instead of actually"
			" performing any action, only displays what would be done", NULL},
		POPT_TABLEEND
	};
	const struct poptOption popts[] = {
		{NULL, 0, POPT_ARG_INCLUDE_TABLE, &actions, 0,
			"Actions (only one can be specified):", NULL},
		{NULL, 0, POPT_ARG_INCLUDE_TABLE, &goptions, 0, "General Options:", NULL},
		{NULL, 0, POPT_ARG_INCLUDE_TABLE, &moptions, 0, "Merge Options"
			" (taken into account when requested action performes"
			" package (un)merging):", NULL},
		POPT_TABLEEND
	};
	poptContext ctx = poptGetContext(NULL, argc, argv, popts,
		POPT_CONTEXT_NO_EXEC);
	poptSetOtherOptionHelp(ctx, "[OPTION...] ACTION [ARGS...]");
	int rc = poptGetNextOpt(ctx);
	int ret = EXIT_FAILURE;
	if (rc == -1) {
		int actions = depclean + help + info + install + search + update
			+ version;

		/*
			Special case for `cmerge foo/bar`.
			We treat it as if --install was specified.
		 */
		if (actions == 0 && poptPeekArg(ctx) != NULL) actions = install = 1;

		if (actions == 0 || help) {
			poptPrintHelp(ctx, stdout, 0);
		}  else if (actions > 1) {
			fputs("Only one action can be given\n", stderr);
		} else if (version) {
			print_version();
			ret = EXIT_SUCCESS;
		} else {
			puts("Action!");
			ret = EXIT_SUCCESS;
		}
	} else {
		/* Invalid option */
		fprintf(stderr, "%s: %s\n",
			poptBadOption(ctx, POPT_BADOPTION_NOALIAS),
			poptStrerror(rc));
	}
	poptFreeContext(ctx);
	return ret;
}
