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

#ifndef CPORTAGE_OBJECT_IMPL_H
#define CPORTAGE_OBJECT_IMPL_H

/** Use this header when you write your own classes */

#include <stddef.h>

#include "cportage/object.h"

#ifdef __cplusplus
extern "C" {
#endif

#pragma GCC visibility push(default)

    struct CPortageObject {
        char _ [sizeof(struct {
            unsigned long magic;
            const struct CPortageClass * klass;
            int refcount;
        })];
    };

    struct CPortageClass {
        char _ [sizeof(struct {
            const struct CPortageObject _;
            const char * name;
            const struct Class * super;
            size_t size;
            void * (* ctor) (void * self, va_list ap);
            void * (* dtor) (void * self);
            void * (* _new) (const void * _class, va_list ap);
        })];
    };

#pragma GCC visibility pop

#ifdef __cplusplus
}
#endif

#endif
