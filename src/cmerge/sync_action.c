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

#include <stdlib.h>

#include "actions.h"

int
cmerge_sync_action(
    CPContext ctx,
    /*@unused@*/ const CMergeOptions options G_GNUC_UNUSED,
    GError **error
) {
    g_assert(error == NULL || *error == NULL);

    CP_GLIST_ITER(cp_settings_repositories(ctx->settings), repo) {
        int retval = cp_repository_sync(repo, error);
        if (retval != EXIT_SUCCESS) {
            return retval;
        }
    } end_CP_GLIST_ITER

    return EXIT_SUCCESS;
}
