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

#include <cportage.h>

/*@-exportany@*/

/* Commandline options */
typedef struct CMergeOptions {
    /* Leftover args */
    /*@null@*/ /*@observer@*/ char **args;
    gboolean pretend;
    gboolean update;
} *CMergeOptions;

typedef struct CPContext {
    CPAtomFactory atom_factory;
    CPSettings settings;
    CPVartree vartree;
    CPTree vardb;
} *CPContext;

int
cmerge_help_action(
    CPContext ctx,
    const CMergeOptions options,
    /*@null@*/ GError **error
) /*@modifies *error,*stdout,errno@*/;

int
cmerge_info_action(
    CPContext ctx,
    const CMergeOptions options,
    /*@null@*/ GError **error
) /*@modifies ctx->settings,*error,*stdout,errno@*/ /*@globals fileSystem@*/;

int
cmerge_install_action(
    CPContext ctx,
    const CMergeOptions options,
    /*@null@*/ GError **error
);

int
cmerge_sync_action(
    CPContext ctx,
    const CMergeOptions options,
    /*@null@*/ GError **error
) /*@modifies *error,*stdout,*stderr,errno,fileSystem@*/;

int
cmerge_version_action(
    CPContext ctx,
    const CMergeOptions options,
    /*@null@*/ GError **error
) /*@modifies *stdout,errno@*/;

#endif
