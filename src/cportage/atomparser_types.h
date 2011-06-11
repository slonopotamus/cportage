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

#include <cportage.h>

/*@-fielduse@*/

struct pv {
    char *package;
    CPVersion version;
};

typedef enum VersionSuffixType {
    SUF_ALPHA,
    SUF_BETA,
    SUF_PRE,
    SUF_RC,
    SUF_P
} VersionSuffixType;

typedef struct VersionSuffix {
    VersionSuffixType type;
    char *value;
} *VersionSuffix;

typedef union YYSTYPE {
    char *str;
    GList/*<char *>*/ *str_list;
    GList/*<VersionSuffix>*/ *suffix_list;
    CPAtom atom;
    CPVersion version;
    struct pv pv;
    VersionSuffixType suffix_type;
    char chr;
} YYSTYPE;

#define YYSTYPE YYSTYPE
