/*
    Copyright 2009-2011, Marat Radchenko

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

#ifndef CMERGE_ACTIONS_H
#define CMERGE_ACTIONS_H

#include <glib.h>

#include <cportage/settings.h>

/*@-exportany@*/

/* Global options */
typedef struct CMergeOptions {
    /* Leftover args */
    /*@null@*/ char **args;
    gboolean pretend;
    gboolean update;
} *CMergeOptions;

int
cmerge_help_action(
    CPSettings settings,
    const CMergeOptions options,
    /*@null@*/ GError **error
) /*@modifies *error,*stdout,errno@*/;

int
cmerge_info_action(
    CPSettings settings,
    const CMergeOptions options,
    /*@null@*/ GError **error
) /*@modifies settings,*error,*stdout,errno@*/ /*@globals fileSystem@*/;

int
cmerge_install_action(
    CPSettings settings,
    const CMergeOptions options,
    /*@null@*/ GError **error
);

int
cmerge_sync_action(
    CPSettings settings,
    const CMergeOptions options,
    /*@null@*/ GError **error
) /*@modifies *error,*stdout,*stderr,errno,fileSystem@*/;

int
cmerge_version_action(
    CPSettings settings,
    const CMergeOptions options,
    /*@null@*/ GError **error
) /*@modifies *stdout,errno@*/;

#endif
