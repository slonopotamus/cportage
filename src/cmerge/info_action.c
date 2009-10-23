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

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>

#include "cportage/atom.h"
#include "cportage/porttree.h"
#include "cportage/settings.h"
#include "cportage/strings.h"

#include "config.h"
#include "cmerge/actions.h"

static void print_version(const struct utsname * utsname) {
	#warning TODO: read current profile from settings
	const char * profile = "default/linux/arm/10.0/desktop";
	#warning TODO: read gcc version from gcc-config
	const char * gcc_ver = "gcc-4.3.2";
	#warning TODO: read libc from vartree
	const char * libc_ver = "glibc-2.8_p20080602-r1";

	printf("cportage %s (%s, %s, %s, %s %s)\n",
		CPORTAGE_VERSION, profile, gcc_ver, libc_ver,
		utsname->release, utsname->machine);
}

static void print_porttree_timestamp(const void * porttree) {
	char * filename = porttree_get_path(porttree, "/metadata/timestamp.chk");
	fputs("Timestamp of tree: ", stdout);
	FILE * f = fopen(filename, "r");
	if (f) {
		char buf[BUFSIZ];
		char * s;
		bool need_newline = true;
		while ((s = fgets(buf, sizeof(buf), f))) {
			need_newline = s[strlen(s) - 1] != '\n';
			fputs(s, stdout);
		}
		fclose(f);
		if (need_newline)
			puts("");
	} else
		puts("Unknown");
	free(filename);
}

static void print_packages(const void * porttree) {
	char * filename = porttree_get_path(porttree, "/profiles/info_pkgs");
	FILE * f = fopen(filename, "r");
	if (f) {
		char buf[BUFSIZ];
		char * s;
		while ((s = fgets(buf, sizeof(buf), f))) {
			trim(s);
			if (s[0] == '#' || s[0] = '\0') {
				continue;
			}
			void * atom = new(Class(Atom), s);
			s = concat(s, ":");
			if (atom) {
				const char * version = "3.2_p39";
				printf("%-20s %s\n", s, version);
			} else {
				printf("%-20s %s\n", s, "[NOT VALID]");
			}
			free(s);
			unref(atom);
		}
		fclose(f);
	}
	free(filename);
}

int info_action(const struct cmerge_gopts * options) {
	void * settings = new(Class(Settings), options->config_root);
	if (!settings)
		return EXIT_FAILURE;

	void * porttree = new(Class(Porttree), settings);
	assert(porttree);

	struct utsname utsname;
	int rc = uname(&utsname);
	assert(rc == 0);
	print_version(&utsname);
	puts("=================================================================");
	fputs("System uname: ", stdout);
	#warning TODO: read cpu name from /proc/cpuinfo
	const char * cpu = "ARMv6-compatible_processor_rev_2_-v6l";
	#warning TODO: read system name from /etc/gentoo-release
	const char * system = "gentoo-1.12.11.1";
	printf("%s-%s-%s-%s-with-%s\n",
		utsname.sysname, utsname.release, utsname.machine, cpu, system);
	print_porttree_timestamp(porttree);
	print_packages(porttree);

	#warning TODO: list vars from $PORTDIR/profiles/info_vars
	for  (int i = 0; i < 1; ++i) {
		const char * var = "ACCEPT_KEYWORDS";
		const char * value = "arm";
		printf("%s=\"%s\"\n", var, value);
	}
	unref(porttree);
	unref(settings);
	return EXIT_SUCCESS;
}
