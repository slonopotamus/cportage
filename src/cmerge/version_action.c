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

#include "actions.h"
#include "config.h"

void
cmerge_version_action(
    CPSettings settings G_GNUC_UNUSED,
    const CMergeOptions options G_GNUC_UNUSED,
    GError **error
) {
    g_assert(error == NULL || *error == NULL);

    g_print("cportage " CP_VERSION "\n\n");
    g_print("Copyright (C) 2009-2010 Marat Radchenko <marat@slonopotamus.org>\n"
         "License GPLv3+: GNU GPL version 3 or later"
             " <http://gnu.org/licenses/gpl.html>\n"
         "This is free software: you are free to change and redistribute it.\n"
         "There is NO WARRANTY, to the extent permitted by law.\n");
}
