#include "answers.h"
#include "gender.h"
#include "stringbuilder.h"

#pragma once

struct actable {
	const char*		name;
	gender_s		gender;
	void			act(const char* format, ...) const { actv(*answers::console, format, xva_start(format), ' '); }
	void			actn(const char* format, ...) const { actv(*answers::console, format, xva_start(format), '\n'); }
	void			actv(stringbuilder& sb, const char* format, const char* format_param, char separator) const;
	const char*		getname() const { return name; }
};
