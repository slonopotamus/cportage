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

#ifndef CPORTAGE_OBJECT_H
#define CPORTAGE_OBJECT_H

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

#pragma GCC visibility push(default)

    /**
        OOP system, heavily based on 'Object-Oriented Programming With ANSI-C' paper.
        Supports inheritance, static/dynamic object methods, typesafe casts.
     */

    /**
        Ondemand class initializarion.
        Always use this macro when first accessing some class outside of instance
        of that class (or its descendant).
     */
#define CPortageClass(x) x ? x : ( x = cportage_init ## x () )

    /** Pointer to Object class. */
    extern const void * CPortageObject;

    /** Pointer to Class class */
    extern const void * CPortageClass;

    /** Increases refcounter of given object */
    void * cportage_ref(void * self);

    /**
        Decreases refcounter of given object, potentially destroying it
        if refcount becomes zero.
     */
    void cportage_unref(void * self);

    /** Type checking cast. Always use it when get objects in params. */
    void * cportage_cast(const void * _class, const void * self);

    /**
        New instance creation function.
        Varargs are specific to class you're instantiating.
     */
    void * cportage_new(const void * _class, ...);

    /** Selector for constructor overriding */
    void * cportage_ctor(void * self, va_list ap);

    /** Selector for destructor overriding */
    void * cportage_dtor(void * self);

    void * cportage_super_ctor(const void * _class, void * self, va_list ap);

    void * cportage_super_dtor(const void * _class, void * self);

    void * cportage_super_new(const void * _class, const void * of, va_list ap);

    void * cportage_alloc(const void * _class);

#pragma GCC visibility pop

#ifdef __cplusplus
}
#endif

#endif
