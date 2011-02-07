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

#include "config.h"

#include <errno.h>
#include <stdlib.h>

#if HAVE_UNISTD_H
#   include <unistd.h>
#endif

#include "actions.h"

int
cmerge_help_action(
    /*@unused@*/ CPSettings settings G_GNUC_UNUSED,
    /*@unused@*/ const CMergeOptions options G_GNUC_UNUSED,
    GError **error
) {
#if HAVE_EXECLP
    int retval;
    int save_errno;

    g_assert(error == NULL || *error == NULL);

    /* TODO: use glib function instead? */
    /*@-moduncon@*/
    retval = execlp("man", "man", "cmerge", NULL);
    /*@=moduncon@*/

    save_errno = errno;
    /*
      We could use own error type but it isn't worth it
      since no sensible handling can be done in calling code anyway.
     */
    g_set_error(error, G_SPAWN_ERROR, (gint)G_SPAWN_ERROR_FAILED,
        _("Can't run '%s': %s"), "man", g_strerror(save_errno));

    return retval;
#else
    g_assert(error == NULL || *error == NULL);

    g_print("See `man cmerge` for documentation");
    return EXIT_SUCCESS;
#endif
}
