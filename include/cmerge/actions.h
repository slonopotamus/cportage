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

#ifndef CMERGE_ACTIONS_H
#define CMERGE_ACTIONS_H

#include "cmerge/options.h"

int cmerge_clean_action(const struct cmerge_mopts * options);

int cmerge_info_action(const struct cmerge_gopts * options);

int cmerge_install_action(const struct cmerge_mopts * options);

int cmerge_search_action(const struct cmerge_gopts * options);

#endif

