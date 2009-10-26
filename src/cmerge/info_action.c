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
#include "cportage/io.h"
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

static void print_timestamp_line(void *ctx, char * s) {
	bool * need_newline = ctx;
	* need_newline = s[strlen(s) - 1] != '\n';
	fputs(s, stdout);
}

static void print_porttree_timestamp(const void * porttree) {
	char * filename = porttree_get_path(porttree, "/metadata/timestamp.chk");
	fputs("Timestamp of tree: ", stdout);
	bool need_newline = true;
	if (processrawlines(filename, &need_newline, &print_timestamp_line)) {
		if (need_newline)
			puts("");
	} else
		puts("Unknown");
	free(filename);
}

static void print_package(void * ctx __attribute__((unused)), char * s) {
	void * atom = new(Class(Atom), s);
	char * s1 = concat(s, ":");
	if (atom) {
		const char * version = "3.2_p39";
		printf("%-20s %s\n", s1, version);
	} else
		printf("%-20s %s\n", s1, "[NOT VALID]");
	free(s1);
	unref(atom);
}

static void print_packages(const void * porttree) {
	char * filename = porttree_get_path(porttree, "/profiles/info_pkgs");
	processlines(filename, NULL, &print_package);
	free(filename);
}

static void print_setting(void * ctx __attribute__((unused)), char * s) {
	#warning TODO: read var value from settings
	printf("%s=\"%s\"\n", s, "arm");
}

static void print_settings(const void * porttree) {
	char * filename = porttree_get_path(porttree, "/profiles/info_vars");
	processlines(filename, NULL, &print_setting);
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
	print_settings(porttree);

	unref(porttree);
	unref(settings);
	return EXIT_SUCCESS;
}
