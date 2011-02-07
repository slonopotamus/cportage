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
#include <cportage/shellconfig.h>

#if HAVE_UNISTD_H
#   include <unistd.h>
#elif HAVE_RESOURCE_H
#   include <sys/resource.h>
#endif

#include "nice.h"

void
adjust_niceness(const CPSettings settings) {
    /*@observer@*/ static const char *key = "PORTAGE_NICENESS";
    const char *value = cp_settings_get(settings, key);
    int inc;

    if (value == NULL) {
        return;
    }
    inc = atoi(value);
    if (inc == 0) {
        return;
    }
    errno = 0;

#if HAVE_NICE
    /*@-moduncon@*/
    inc = nice(inc);
    /*@=moduncon@*/
#elif HAVE_GETPRIORITY && HAVE_SETPRIORITY
    inc += getpriority(PRIO_PROCESS, 0);
    if (errno) {
        int save_errno = ernno;
        g_warning(_("Can't get current process priority: %s"), g_strerror(save_errno));
        return;
    }
    setpriority(PRIO_PROCESS, 0, inc);
#else
    g_warning(_("%s is specified but system doesn't have neither nice()"
        " nor getpriority()/setpriority() functions"), key);
#endif

    if (errno != 0) {
        int save_errno = errno;
        g_warning(_("Can't change nice value to '%s': %s"),
            value, g_strerror(save_errno));
    }
}

void
adjust_ionice(const CPSettings settings) {
    /*@observer@*/ static const char *key = "PORTAGE_IONICE_COMMAND";
    const char *raw_value = cp_settings_get(settings, key);

    if (raw_value == NULL) {
        return;
    }

#if HAVE_GETPID
    {
#ifndef S_SPLINT_S
        G_STATIC_ASSERT(sizeof(pid_t) <= sizeof(long));
#endif
        GError *error = NULL;
        GHashTable *vars = NULL;
        char *cmd = NULL;
        int retval;
        /*@-moduncon@*/
        pid_t pid = getpid();
        /*@=moduncon@*/

        vars = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
        g_hash_table_insert(vars,
            g_strdup("PID"),
            g_strdup_printf("%ld", (long)pid));
        cmd = cp_varexpand(raw_value, vars, &error);
        if (cmd == NULL) {
            goto ERR;
        }
        /*@-modfilesys@*/
        if (!g_spawn_command_line_sync(cmd, NULL, NULL, &retval, &error)) {
            goto ERR;
        }
        /*@=modfilesys@*/
        if (retval != EXIT_SUCCESS) {
            g_warning(_("Command '%s' returned %d"), cmd, retval);
		        g_warning(_("See the make.conf(5) man page for %s usage instructions."), key);
        }

ERR:
        if (error != NULL) {
            g_warning(_("Can't run '%s': %s"), cmd, error->message);
            g_error_free(error);
        }
        g_free(cmd);
        if (vars != NULL) {
            g_hash_table_destroy(vars);
        }
    }
#else
    g_warning(_("%s is specified but system doesn't have getpid() function"), key);
#endif
}
