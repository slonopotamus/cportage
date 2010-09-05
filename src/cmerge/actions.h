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
#include <stdbool.h>

/*@-exportany@*/

typedef enum {
    VERBOSITY_QUIET = -1,
    VERBOSITY_NORMAL = 0,
    VERBOSITY_VERBOSE = 1
} VerbosityLevel;

/* Global options */
typedef struct GlobalOptions {
    /*@observer@*/ const char *config_root;
    /* Leftover args */
    /*@null@*/ char * const *args;
    VerbosityLevel verbosity;
} *GlobalOptions;

/* Merge/unmerge options */
typedef struct MergeOptions {
    /*@observer@*/ GlobalOptions global;
    int pretend;
    int update;
} *MergeOptions;

void
cmerge_clean_action(
    const MergeOptions options,
    bool with_deps,
    /*@null@*/ GError **error
)
    /*@modifies *error@*/;

void
cmerge_info_action(
    const GlobalOptions options,
    /*@null@*/ GError **error
)
    /*@globals stdout@*/
    /*@modifies fileSystem,errno,*stdout,*error@*/;

void
cmerge_install_action(
    const MergeOptions options,
    /*@null@*/ GError **error
)
    /*@modifies *error@*/;

void
cmerge_search_action(
    const GlobalOptions options,
    /*@null@*/ GError **error
)
    /*@modifies *error@*/;

#endif
