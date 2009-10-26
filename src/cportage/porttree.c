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

/* Required for stat() */
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "cportage/object.r"
#include "cportage/porttree.h"
#include "cportage/settings.h"
#include "cportage/strings.h"

struct Porttree {
	struct Object _;
	char * portdir;
};

const void * Porttree;

char * porttree_get_path(const void * _self, const char * relative) {
	struct Porttree * self = cast(Porttree, _self);
	assert(relative && relative[0] == '/');
	return concat(self->portdir, relative);
}

static void * Porttree_ctor(void * _self, va_list ap) {
	super_ctor(Porttree, _self, ap);
	struct Porttree * self = cast(Porttree, _self);
	const void * settings = cast(Class(Settings), va_arg(ap, void *));

	self->portdir = settings_get_default(settings, "PORTDIR", "/usr/portage");
	assert(self->portdir);
	const size_t len = strlen(self->portdir);
	/* Remove trailing slashes */
	for (size_t i = len; i > 1 && self->portdir[i - 1] == '/'; --i)
		self->portdir[i - 1] = '\0';
	/* Check portdir */
	struct stat buf;
	if (stat(self->portdir, &buf)) {
		fprintf(stderr, "ERROR: Could not access PORTDIR %s: ", self->portdir);
		perror(NULL);
	} else if (!S_ISDIR(buf.st_mode)) {
		fprintf(stderr, "ERROR: PORTDIR %s is not a directory\n", self->portdir);
	}

	return self;
}

static void * Porttree_dtor(void * _self) {
	struct Porttree * self = cast(Porttree, _self);
	free(self->portdir);
	return super_dtor(Porttree, self);
}

void * initPorttree() {
	return new(Class, "Porttree", Object, sizeof(struct Porttree),
		ctor, Porttree_ctor,
		dtor, Porttree_dtor);
}
