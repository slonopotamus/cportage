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

#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#include "actions.h"

int
cmerge_help_action(
    CPSettings settings G_GNUC_UNUSED,
    const CMergeOptions options G_GNUC_UNUSED,
    GError **error
) {
    int retval;
    int save_errno;

    g_assert(error == NULL || *error == NULL);

    /* TODO: use glib function instead? */
    retval = execlp("man", "man", "cmerge", NULL);

    save_errno = errno;
    /*
      We could use own error type but it isn't worth it
      since no sensible handling can be done in calling code anyway.
     */
    g_set_error(error, G_SPAWN_ERROR, G_SPAWN_ERROR_FAILED,
        _("Can't run '%s': %s"), "man", g_strerror(save_errno));

    return retval;
}
