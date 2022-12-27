#include "gender.h"
#include "stringbuilder.h"

#pragma once

extern stringbuilder sb;

class actable {
	short unsigned	type, name;
	gender_s		gender;
public:
	void			act(const char* format, ...) const { actv(sb, format, xva_start(format), '\n'); }
	void			actv(stringbuilder& sb, const char* format, const char* format_param, char separator) const;
	gender_s		getgender() const { return gender; }
	const char*		getname() const;
};