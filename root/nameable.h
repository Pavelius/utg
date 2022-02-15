#include "crt.h"
#include "gender.h"
#include "stringbuilder.h"

#pragma once

struct nameable {
	const char*		id;
	gender_s		gender;
	void			actv(stringbuilder& sb, const char* format, const char* param, char add_sep = ' ') const;
	const char*		getname() const { return getnm(id); }
	gender_s		getgender() const { return gender; }
};