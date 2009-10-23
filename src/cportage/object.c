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
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cportage/object.h"

#define MAGIC 0xCAFEBABE
#define isObject(p) (assert(p), assert(((struct Object *)p)->magic == MAGIC), p)

/** Keep in sync with object.r */
struct Object {
	unsigned long magic;
	const struct Class * class;
	int refcount;
};

struct Class {
	const struct Object _;
	const char * name;
	const struct Class * super;
	size_t size;
	void * (* ctor) (void * self, va_list * app);
	void * (* dtor) (void * self);
	void * (* new) (const void * class, va_list * app);
};

static const void * classOf(const void * _self) {
	const struct Object * self = isObject(_self);
	assert(self->class);
	return self->class;
}

void * ctor(void * self, va_list * app) {
	const struct Class * class = classOf(self);
	assert(class->ctor);
	return class->ctor(self, app);
}

void *dtor(void * self) {
	const struct Class * class = classOf(self);
	assert(class->dtor);
	return class->dtor(self);
}

void * ref(void * _self) {
	struct Object * self = isObject(_self);
	++self->refcount;
	return self;
}

void unref(void * _self) {
	if (_self) {
		struct Object * self = isObject(_self);
		--self->refcount;
		if (!self->refcount)
			free(dtor(self));
	}
}

static size_t sizeOf(const void * self) {
	const struct Class * class = classOf(self);
	assert(class->size);
	return class->size;
}

/*
static bool isA(const void * self, const void * class) {
	return self && classOf(self) == cast(Class, class);
}
*/

static const void * super(const void *_self) {
	const struct Class * self = cast(Class, _self);
	return self->super;
}

static bool isOf(const void * self, const void * class) {
	if (self) {
		assert(isObject(self));
		const struct Class * myClass = classOf(self);
		while (myClass) {
			if (myClass == class)
				return true;
			myClass = myClass->super;
		}
		return false;
	}
	return false;
}

void * cast(const void * class, const void * self) {
	assert(isOf(self, class));
	return (void *)self;
}

static void * Object_ctor(void * self, va_list * app __attribute__((unused))) {
	return cast(Object, self);
}

static void * Object_dtor(void * self) {
	return cast(Object, self);
}

void * alloc(const void * _class) {
	const struct Class * class = cast(Class, _class);
	assert(class->size >= sizeof(struct Object));
	struct Object * object = calloc(1, class->size);
	assert(object);
	object->magic = MAGIC;
	object->class = class;
	object->refcount = 1;
	return object;
}

static void * Object_new(const void * _class, va_list * app) {
	const struct Class * class = cast(Class, _class);
	return ctor(alloc(class), app);
}

void * new(const void * _class, ...) {
	const struct Class * class = cast(Class, _class);
	assert(class->new);
	va_list ap;
	va_start(ap, _class);
	struct Object * object = class->new(class, &ap);
	va_end(ap);
	return object;
}

void * super_ctor(const void * class, void * self, va_list * app) {
	const struct Class * superclass = super(class);
	assert(superclass && superclass->ctor);
	return superclass->ctor(cast(Object, self), app);
}

void * super_dtor(const void * class, void * self) {
	const struct Class * superclass = super(class);
	assert(superclass && superclass->dtor);
	return superclass->dtor(cast(Object, self));
}

void * super_new(const void * class, const void * of, va_list * app) {
	const struct Class * superclass = super(class);
	assert(superclass && superclass->new);
	return superclass->new(of, app);
}

static void * Class_ctor(void * _self, va_list * app) {
	struct Class * self = cast(Class, _self);
	self->name  = va_arg(* app, char *);
	self->super = cast(Class, va_arg(* app, struct Class *));
	self->size  = va_arg(* app, size_t);
	assert(self->super);

	/* Copy over stuff from super class */
	const size_t offset = offsetof(struct Class, ctor);
	memcpy((char *)self + offset,
		(char*)self->super + offset,
		sizeOf(self->super) - offset);

	/* Handle dynamic overrides */
	typedef void (* voidf) ();
	voidf selector;
	va_list ap = * app;
	while ((selector = va_arg(ap, voidf))) {
		voidf method = va_arg(ap, voidf);
		if (selector == (voidf) ctor)
			* (voidf *) &self->ctor = method;
		else if (selector == (voidf) dtor)
			* (voidf *) &self->dtor = method;
		else if (selector == (voidf) new)
			* (voidf *) &self->new = method;
	}
	return self;
}

static void * Class_dtor(void * _self) {
	const struct Class * self = cast(Class, _self);
	fprintf(stderr, "%s: cannot destroy class\n", self->name);
	return NULL;
}

static const struct Class object[] = {
	{
		{ MAGIC, &object[1], 1}, "Object", NULL, sizeof(struct Object),
		Object_ctor, Object_dtor, Object_new
	},
	{
		{ MAGIC, &object[1], 1}, "Class", &object[0], sizeof(struct Class),
		Class_ctor, Class_dtor, Object_new
	}
};

const void * Object = &object[0];

const void * Class = &object[1];
