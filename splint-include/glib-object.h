#include <glib.h>

#ifndef GLIB_OBJECT_H
#define GLIB_OBJECT_H

/* gobject.h */

void
g_object_unref(/*@only@*/ gpointer object) /*@modifies object@*/;

/* gtype.h */

void
g_type_init(void) /*@modifies internalState@*/;

#endif
