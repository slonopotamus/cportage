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
#include <pcre.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cportage/atom.h"
#include "cportage/object.r"

struct Atom {
	struct Object _;
	const char * category;
	const char * package;
	/* Nullable */
	const char * version;
	/* Nullable */
	const char * operator;
	/* Nullable */
	const char * slot;
};

struct AtomClass {
	const struct Class _;
	pcre * atom_re;
	pcre_extra * atom_re_extra;
	int atom_re_nsub;
};

extern const void * AtomClass;

static void * Atom_new(const void * _class, va_list * app) {
	const struct AtomClass * class = cast(AtomClass, _class);
	const char * s = va_arg(* app, char *);
	assert(s);
	assert(class->atom_re);

	/* See man 3 pcreapi "How pcre_exec() returns captured substrings" */
	int m_data[(class->atom_re_nsub + 1) * 3];

	const int m = pcre_exec(class->atom_re, class->atom_re_extra,
		s, strlen(s), 0, 0, m_data, sizeof(m_data));
	if (m == PCRE_ERROR_NOMATCH) {
		return NULL;
	}
	assert(m > 0);
	#warning TODO: calculate these once when regex is compiled?
	const int op_idx = pcre_get_stringnumber(class->atom_re, "op");
	const int star_idx = pcre_get_stringnumber(class->atom_re, "star");
	const int simple_idx = pcre_get_stringnumber(class->atom_re, "simple");
	const int slot_idx = pcre_get_stringnumber(class->atom_re, "slot");
	const int use_idx = pcre_get_stringnumber(class->atom_re, "use");
	assert(op_idx > 0 && star_idx > 0 && simple_idx > 0 && slot_idx > 0
		&& use_idx > 0);

	const char * type;
	int rc, cat_idx;
	if ((rc = pcre_get_substring(s, m_data, m, op_idx, &type)) > 0) {
		cat_idx = op_idx + 2;
	} else if ((rc = pcre_get_substring(s, m_data, m, star_idx, &type)) > 0) {
		cat_idx = star_idx + 2;
	} else if ((rc = pcre_get_substring(s, m_data, m, simple_idx, &type)) > 0) {
		cat_idx = simple_idx + 2;
	} else {
		/* Should never happen (if we get here, we have a error in regex) */
		assert(false);
	}
	pcre_free_substring(type);
	const char * invalid_version;
	rc = pcre_get_substring(s, m_data, m, cat_idx + 2, &invalid_version);
	if (rc > 0) {
		/* Pkg name ends with version string, that's disallowed */
		pcre_free_substring(invalid_version);
		return NULL;
	}
	struct Atom * atom = super_ctor(Atom, alloc(class), NULL);
	rc = pcre_get_substring(s, m_data, m, cat_idx, &atom->category);
	assert(rc > 0);
	rc = pcre_get_substring(s, m_data, m, cat_idx + 1, &atom->package);
	assert(rc > 0);
	pcre_get_named_substring(class->atom_re, s, m_data, m, "slot", &atom->slot);
	#warning TODO: store version, useflags and operator
	return atom;
}

static void * Atom_dtor(void *_self) {
	struct Atom * self = cast(Atom, _self);
	pcre_free_substring(self->category);
	pcre_free_substring(self->package);
	pcre_free_substring(self->slot);
	return self;
}

static void * AtomClass_ctor(void * _self, va_list * app) {
	super_ctor(AtomClass, _self, app);
	struct AtomClass * self = cast(AtomClass, _self);
	/*
		2.1.1 A category name may contain any of the characters [A-Za-z0-9+_.-].
		It must not begin with a hyphen or a dot.
	 */
	const char * cat  = "([\\w+][\\w+.-]*)";
	/*
		2.1.2 A package name may contain any of the characters [A-Za-z0-9+_-].
		It must not begin with a hyphen,
		and must not end in a hyphen followed by _valid version string_.
	*/
	const char * pkg  = "([\\w+][\\w+-]*?)";
	/*
		2.1.3 A slot name may contain any of the characters [A-Za-z0-9+_.-].
		It must not begin with a hyphen or a dot.
	 */
	const char * slot = "(?P<slot>[\\w+][\\w+.-]*)";
	/*
		2.1.4 A USE flag name may contain any of the characters [A-Za-z0-9+_@-].
		It must begin with an alphanumeric character.
	 */
	const char * use_name = "[A-Za-z0-9][\\w+@-]*";
	/* See 2.2 section for version syntax. */
	const char * ver  = "(cvs\\.)?(\\d+)((\\.\\d+)*)([a-z]?)"
		"((_(pre|p|beta|alpha|rc)\\d*)*)(-r\\d+)?";
	#warning TODO: add reference to PMS
	const char * op   = "(?P<op>[=~]|[><]=?)";

	char * use, * use_item, * cp, * cpv, * atom_re_str;
	if (asprintf(&use_item, "(?:!?%s[=?]|-?%s)",
		use_name, use_name) == -1) abort();
	if (asprintf(&use, "(?P<use>\\[%s(?:,%s)*\\])?",
		use_item, use_item) == -1) abort();
	if (asprintf(&cp, "(%s/%s(-%s)\?\?)", cat, pkg, ver) == -1) abort();
	if (asprintf(&cpv, "%s-%s", cp, ver) == -1) abort();
	if (asprintf(&atom_re_str,
		"^(?:(?:%s%s)|(?P<star>=%s\\*)|(?P<simple>%s))(?::%s)?%s$",
		op, cpv, cpv, cp, slot, use) == -1) abort();
	free(use);
	free(cp);
	free(cpv);
	const char *err;
	int erroffset;
	self->atom_re = pcre_compile(atom_re_str, 0, &err, &erroffset, NULL);
	if (!self->atom_re) {
		fprintf(stderr, "Atom regex compilation failed at offset %d: %s",
			erroffset, err);
		abort();
	}
	free(atom_re_str);
	self->atom_re_extra = pcre_study(self->atom_re, 0, &err);
	if (err) {
		fprintf(stderr, "Atom regex study failed: %s", err);
		abort();
	}
	const int rc = pcre_fullinfo(self->atom_re, self->atom_re_extra,
		PCRE_INFO_CAPTURECOUNT, &self->atom_re_nsub);
	assert(rc == 0);
	return self;
}

const void * AtomClass;

static void * initAtomClass() {
	return new(Class, "AtomClass", Class, sizeof(struct AtomClass),
		ctor, AtomClass_ctor);
}

const void * Atom;

void * initAtom() {
	return new(Class(AtomClass), "Atom", Object, sizeof(struct Atom),
		new , Atom_new,
		dtor, Atom_dtor);
}
