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
#include <string.h>

#include "cportage/io.h"
#include "cportage/strings.h"

bool getrawlines(const char * filename,
		void * ctx,
		void (* func) (void *ctx, char * s)) {
	FILE * f = fopen(filename, "r");
	if (!f) return false;

	size_t bufsize, i = 0;
	ssize_t len;
	char * s = NULL;
	while ((len = getline(&s, &bufsize, f)) != -1) {
		++i;
		if (strlen(s) == (size_t)len)
			func(ctx, s);
		else
			fprintf(stderr, "null byte in %s on line %zd, skipping line\n",
				filename, i);
	}
	free(s);
	fclose(f);
	return true;
}

struct _getlinectx {
	void * orig_ctx;
	void (* orig_func) (void * ctx, char * s);
};

static void _getline(void * _ctx, char * s) {
	char * comment = strchr(s, '#');
	if (comment)
		* comment = '\0';
	trim(s);
	if (s[0] != '\0') {
		struct _getlinectx * ctx = _ctx;
		ctx->orig_func(ctx->orig_ctx, s);
	}
}

bool getlines(const char * filename,
		void * orig_ctx,
		void (* func) (void * ctx, char * s)) {
	struct _getlinectx ctx = { orig_ctx, func };
	return getrawlines(filename, &ctx, &_getline);
}
