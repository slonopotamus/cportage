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

#include "shellconfig.h"

/*@-fielduse@*/

#ifndef YY_TYPEDEF_YY_SCANNER_T
#define YY_TYPEDEF_YY_SCANNER_T
typedef void *yyscan_t;
#endif

typedef struct cp_shell_parser_ctx_t {
    yyscan_t yyscanner;
    const char *filename;
    void *entries;
    CPShellconfigLookupFunc lookup_func;
    CPShellconfigSaveFunc save_func;
    char *expanded;
    GError **error;
    int magic;
    gboolean allow_source;
} /*@unused@*/ cp_shell_parser_ctx;
