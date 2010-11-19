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

void
cmerge_sync_action(
    CPSettings settings,
    const CMergeOptions options G_GNUC_UNUSED,
    GError **error
) {
    g_assert(error == NULL || *error == NULL);

    CP_REPOSITORY_ITER(cp_settings_get_repositories(settings), repo)
        if (!cp_repository_sync(repo, error)) {
            return;
        }
    end_CP_REPOSITORY_ITER
}
