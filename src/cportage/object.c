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

#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cportage/object.h"

#define MAGIC 0xCAFEBABE
#define isObject(p) (assert(p), assert(((struct CPortageObject *)p)->magic == MAGIC), p)

/** Keep in sync with object.r */
struct CPortageObject {
    unsigned long magic;
    const struct CPortageClass * klass;
    int refcount;
};

struct CPortageClass {
    const struct CPortageObject _;
    const char * name;
    const struct CPortageClass * super;
    size_t size;
    void * (* ctor) (void * self, va_list ap);
    void * (* dtor) (void * self);
    void * (* _new) (const void * _class, va_list ap);
};

static const void * classOf(const void * _self) {
    const struct CPortageObject * self = isObject(_self);
    assert(self->klass);
    return self->klass;
}

void * cportage_ctor(void * self, va_list ap) {
    const struct CPortageClass * klass = classOf(self);
    assert(klass->ctor);
    return klass->ctor(self, ap);
}

void * cportage_dtor(void * self) {
    const struct CPortageClass * klass = classOf(self);
    assert(klass->dtor);
    return klass->dtor(self);
}

void * cportage_ref(void * _self) {
    struct CPortageObject * self = isObject(_self);
    ++self->refcount;
    return self;
}

void cportage_unref(void * _self) {
    if (_self) {
        struct CPortageObject * self = isObject(_self);
        --self->refcount;
        if (!self->refcount)
            free(cportage_dtor(self));
    }
}

static size_t sizeOf(const void * self) {
    const struct CPortageClass * klass = classOf(self);
    assert(klass->size);
    return klass->size;
}

/*
static bool isA(const void * self, const void * class) {
    return self && classOf(self) == cast(Class, class);
}
*/

static const void * super(const void *_self) {
    const struct CPortageClass * self = cportage_cast(CPortageClass, _self);
    return self->super;
}

static bool isOf(const void * self, const void * klass) {
    if (self) {
        assert(isObject(self));
        const struct CPortageClass * myClass = classOf(self);
        while (myClass) {
            if (myClass == klass)
                return true;
            myClass = myClass->super;
        }
        return false;
    }
    return false;
}

void * cportage_cast(const void * klass, const void * self) {
    assert(isOf(self, klass));
    return (void *)self;
}

static void * Object_ctor(void * self, va_list ap __attribute__((unused))) {
    return cportage_cast(CPortageObject, self);
}

static void * Object_dtor(void * self) {
    return cportage_cast(CPortageObject, self);
}

void * cportage_alloc(const void * _class) {
    const struct CPortageClass * klass = cportage_cast(CPortageClass, _class);
    assert(klass->size >= sizeof(struct CPortageObject));
    struct CPortageObject * object = calloc(1, klass->size);
    assert(object);
    object->magic = MAGIC;
    object->klass = klass;
    object->refcount = 1;
    return object;
}

static void * Object_new(const void * _class, va_list ap) {
    const struct CPortageClass * klass = cportage_cast(CPortageClass, _class);
    return cportage_ctor(cportage_alloc(klass), ap);
}

void * cportage_new(const void * _class, ...) {
    const struct CPortageClass * klass = cportage_cast(CPortageClass, _class);
    assert(klass->_new);
    va_list ap;
    va_start(ap, _class);
    struct Object * object = klass->_new(klass, ap);
    va_end(ap);
    return object;
}

void * cportage_super_ctor(const void * klass, void * self, va_list ap) {
    const struct CPortageClass * superclass = super(klass);
    assert(superclass && superclass->ctor);
    return superclass->ctor(cportage_cast(CPortageObject, self), ap);
}

void * cportage_super_dtor(const void * klass, void * self) {
    const struct CPortageClass * superclass = super(klass);
    assert(superclass && superclass->dtor);
    return superclass->dtor(cportage_cast(CPortageObject, self));
}

void * cportage_super_new(const void * klass, const void * of, va_list ap) {
    const struct CPortageClass * superclass = super(klass);
    assert(superclass && superclass->_new);
    return superclass->_new(of, ap);
}

static void * Class_ctor(void * _self, va_list ap) {
    struct CPortageClass * self = cportage_cast(CPortageClass, _self);
    self->name  = va_arg(ap, char *);
    self->super = cportage_cast(CPortageClass, va_arg(ap, struct CPortageClass *));
    self->size  = va_arg(ap, size_t);
    assert(self->super);

    /* Copy over stuff from super class */
    const size_t offset = offsetof(struct CPortageClass, ctor);
    memcpy((char *)self + offset,
           (char*)self->super + offset,
           sizeOf(self->super) - offset);

    /* Handle dynamic overrides */
    typedef void (* voidf) (void);
    voidf selector;
    while ((selector = va_arg(ap, voidf))) {
        voidf method = va_arg(ap, voidf);
        if (selector == (voidf) cportage_ctor)
            * (voidf *) &self->ctor = method;
        else if (selector == (voidf) cportage_dtor)
            * (voidf *) &self->dtor = method;
        else if (selector == (voidf) cportage_new)
            * (voidf *) &self->_new = method;
    }
    return self;
}

static void * Class_dtor(void * _self) {
    const struct CPortageClass * self = cportage_cast(CPortageClass, _self);
    fprintf(stderr, "%s: cannot destroy class\n", self->name);
    return NULL;
}

static const struct CPortageClass object[] = {
    {
        { MAGIC, &object[1], 1}, "Object", NULL, sizeof(struct CPortageObject),
        Object_ctor, Object_dtor, Object_new
    },
    {
        { MAGIC, &object[1], 1}, "Class", &object[0], sizeof(struct CPortageClass),
        Class_ctor, Class_dtor, Object_new
    }
};

const void * CPortageObject = &object[0];

const void * CPortageClass = &object[1];
