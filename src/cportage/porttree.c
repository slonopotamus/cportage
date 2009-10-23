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

static void * Porttree_ctor(void * _self, va_list * app) {
	super_ctor(Porttree, _self, app);
	struct Porttree * self = cast(Porttree, _self);
	const void * settings = cast(Class(Settings), va_arg(* app, void *));
	#warning TODO: read portdir from settings
	const int rc = asprintf(&self->portdir, "%s", "/usr/portage");
	assert(rc != -1);
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