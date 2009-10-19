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

#ifndef CPORTAGE_OBJECT_R
#define CPORTAGE_OBJECT_R

/* Use this header when you write your own classes */

#include "cportage/object.h"

#pragma GCC visibility push(default)

struct Object {
	char _ [sizeof(struct {
		unsigned long magic;
		const void * class;
		int refcount;
	})];
};

struct Class {
	char _ [sizeof(struct {
		const struct Object _;
		const char * name;
		const struct Class * super;
		size_t size;
		void * (* ctor) (void * self, va_list * app);
		void * (* dtor) (void * self);
		void * (* new) (const void * class, va_list * app);
		int    (* cmp)  (const void * self, const void * other);
	})];
};

/* Selector for constructor overriding */
void * ctor(void * self, va_list * app);

/* Selector for destructor overriding */
void * dtor(void * self);

void * super_ctor(const void * class, void * self, va_list * app);

void * super_dtor(const void * class, void * self);

void * super_new(const void * class, const void * of, va_list * app);

void * alloc(const void * _class);

#pragma GCC visibility pop

#endif
