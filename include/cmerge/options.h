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

#ifndef CMERGE_OPTIONS_H
#define CMERGE_OPTIONS_H

enum VERBOSITY_LEVEL {
    VERBOSITY_QUIET = -1,
    VERBOSITY_NORMAL = 0,
    VERBOSITY_VERBOSE = 1
};

/* Global options */
struct cmerge_gopts {
    /* Verbosity level. See VERBOSITY_LEVEL for possible values */
    int verbosity;
    const char * config_root;
    /* Leftover args, as returned by poptGetArgs */
    const char ** args;
};

/* Merge/unmerge options */
struct cmerge_mopts {
    /* Reference to general options */
    const struct cmerge_gopts * gopts;
    int pretend;
    int update;
};

#endif

