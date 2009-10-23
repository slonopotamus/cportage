/*
	Copyright 2009, Marat Radchenko

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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "cportage/strings.h"

struct item {
	const char * s;
	const char * expected;
};

int main() {
	struct item data[] = {
		{ "", "" },
		{ " ", "" },
		{ "  ", "" },
		{ "a", "a" },
		{ " a", "a" },
		{ "a ", "a" },
		{ " a ", "a" },
		{ "  a  ", "a" },
	};
	int retval = 0;
	for (unsigned int i = 0; i < sizeof(data) / sizeof(data[0]); ++i) {
		char * s = strdup(data[i].s);
		trim(s);
		if (strcmp(s, data[i].expected) != 0) {
			fprintf(stderr, "trim('%s'), expected '%s' but got '%s'\n",
				data[i].s, data[i].expected, s);
			--retval;
		}
		free(s);
	}
	return retval;
}
