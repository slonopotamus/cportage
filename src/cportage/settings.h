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

#ifndef CP_SETTINGS_H
#define CP_SETTINGS_H

#include <cportage.h>

/*@-exportany@*/

/**
 * \return readonly canonical path to settings root
 */
/*@observer@*/ const char *
cp_settings_config_root(const CPSettings self) G_GNUC_WARN_UNUSED_RESULT /*@*/;

/**
 * \return %TRUE if \a feature is enabled in \c FEATURES variable,
 *         %FALSE otherwise
 */
gboolean
cp_settings_feature_enabled(
    const CPSettings self,
    const char *feature
) G_GNUC_WARN_UNUSED_RESULT /*@*/;

#endif
