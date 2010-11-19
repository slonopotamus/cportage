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

#ifndef CMERGE_ACTIONS_H
#define CMERGE_ACTIONS_H

#include <glib.h>

#include <cportage/settings.h>

/*@-exportany@*/

typedef enum {
    VERBOSITY_QUIET = -1,
    VERBOSITY_NORMAL = 0,
    VERBOSITY_VERBOSE = 1
} VerbosityLevel;

/* Global options */
typedef struct CMergeOptions {
    /* Leftover args */
    /*@null@*/ char * const *args;
    VerbosityLevel verbosity;
    gboolean pretend;
    gboolean update;
} *CMergeOptions;

typedef void (*CMergeAction) (
    CPSettings settings,
    const CMergeOptions options,
    /*@null@*/ GError **error
);

void
cmerge_help_action(
    CPSettings settings,
    const CMergeOptions options,
    /*@null@*/ GError **error
);

void
cmerge_info_action(
    CPSettings settings,
    const CMergeOptions options,
    /*@null@*/ GError **error
) /*@globals stdout@*/ /*@modifies fileSystem,errno,*stdout,*error@*/;

void
cmerge_install_action(
    CPSettings settings,
    const CMergeOptions options,
    /*@null@*/ GError **error
);

void
cmerge_sync_action(
    CPSettings settings,
    const CMergeOptions options,
    /*@null@*/ GError **error
) /*@modifies *error@*/;

void
cmerge_version_action(
    CPSettings settings,
    const CMergeOptions options,
    /*@null@*/ GError **error
);

#endif
