#include "answers.h"
#include "gender.h"
#include "stringbuilder.h"

#pragma once

extern answers an;
extern stringbuilder sb;
extern adat<const void*, 32> marked;

class actable {
	gendern		gender;
public:
	static void	actv(stringbuilder& sb, const char* format, const char* format_param, char separator, const char* name, gendern gender);
	gendern		getgender() const { return gender; }
	void		setgender(gendern v) { gender = v; }
};
bool ismarked(const void* p);
void markvalue(const void* p);