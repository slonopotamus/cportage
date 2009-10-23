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

#ifndef CPORTAGE_OBJECT_H
#define CPORTAGE_OBJECT_H

/*
	OOP system, heavily based on 'Object-Oriented Programming With ANSI-C' paper.
	Supports inheritance, static/dynamic object methods, typesafe casts.
 */

/*
	Ondemand class initializarion.
	Always use this macro when first accessing some class outside of instance
	of that class (or its descendant).
 */
#define Class(x) x ? x : ( x = init ## x () )

#pragma GCC visibility push(default)

/* Pointer to Object class. */
extern const void * Object;

/* Pointer to Class class */
extern const void * Class;

/* Increases refcounter of given object */
void * ref(void * self);

/*
	Decreases refcounter of given object, potentially destroying it
	if refcount becomes zero.
 */
void unref(void * self);

/* Type checking cast. Always use it when get objects in params. */
void * cast(const void * class, const void * self);

/*
	New instance creation function.
	Varargs are specific to class you're instantiating.
 */
void * new(const void * class, ...);

#pragma GCC visibility pop

#endif
