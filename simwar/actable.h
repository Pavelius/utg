#include "gender.h"
#include "nameable.h"

#pragma once

class stringbuilder;

struct actable : nameable {
	gender_s		gender;
	void			act(stringbuilder& sb, const char* format, ...) const;
};