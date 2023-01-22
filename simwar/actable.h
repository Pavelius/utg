#include "gender.h"
#include "nameable.h"
#include "stringbuilder.h"

#pragma once

struct actable : nameable {
	gender_s		gender;
	void			act(stringbuilder& sb, const char* format, ...) const;
	static void		actv(stringbuilder& sb, const char* format, const char* format_param, const char* name, gender_s gender);
};