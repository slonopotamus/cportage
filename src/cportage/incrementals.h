/*
    Copyright 2009-2014, Marat Radchenko

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

#ifndef CP_INCREMENTALS_H
#define CP_INCREMENTALS_H

#include <glib.h>

/*@-exportany@*/

typedef struct CPIncrementals *CPIncrementals;

/*@only@*/ CPIncrementals
cp_incrementals_new(
    /*@dependent@*/ GTree *config
) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT /*@modifies *config@*/;

void
cp_incrementals_destroy(
    /*@null@*/ /*@only@*/ CPIncrementals self
) /*@modifies self@*/;

gboolean
cp_incrementals_process_profile(
    CPIncrementals self,
    const char *dir,
    /*@null@*/ GError **error
) G_GNUC_WARN_UNUSED_RESULT
/*@modifies *self,*error,errno@*/ /*@globals fileSystem@*/;

void
cp_incrementals_config_changed(
    CPIncrementals self,
    gboolean stack_use_expand
) /*@modifies *self@*/;

void
cp_incrementals_config_finished(CPIncrementals self) /*@modifies *self@*/;

gboolean
cp_incrementals_contains(
    CPIncrementals self,
    const char *key,
    const char *value
) G_GNUC_WARN_UNUSED_RESULT /*@*/; 

#endif
