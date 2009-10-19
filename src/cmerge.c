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

#include "cportage/atom.h"
#include "cportage/settings.h"

int main(const int argc, const char * argv[]) {
	const struct poptOption actions[] = {
		POPT_TABLEEND
	};
	char * config_root = "/";
	const struct poptOption options[] = {
		{"config-root", 0, POPT_ARG_STRING, &config_root, 0, "Specifies the location for various portage configuration files", config_root},
		POPT_TABLEEND
	};
	const struct poptOption popts[] = {
		{NULL, 0, POPT_ARG_INCLUDE_TABLE, &actions, 0, "Actions:", NULL},
		{NULL, 0, POPT_ARG_INCLUDE_TABLE, &options, 0, "Options:", NULL},
		POPT_AUTOHELP
		POPT_TABLEEND
	};
	poptContext ctx = poptGetContext(NULL, argc, argv, popts, POPT_CONTEXT_NO_EXEC);
	poptSetOtherOptionHelp(ctx, "[OPTION...] ACTION [ARGS...]");
	int rc = poptGetNextOpt(ctx);
	int ret = 42;
	if (rc == -1) {
		void * settings = new(Class(Settings), config_root);

		unref(settings);
		ret = EXIT_SUCCESS;
	} else {
		/* Invalid option */
		fprintf(stderr, "%s: %s\n",
			poptBadOption(ctx, POPT_BADOPTION_NOALIAS),
			poptStrerror(rc));
		ret = EXIT_FAILURE;
	}
	assert(ret != 42);
	poptFreeContext(ctx);
	return ret;
}
