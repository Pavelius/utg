#include "answers.h"
#include "gender.h"
#include "stringbuilder.h"

#pragma once

extern answers an;
extern stringbuilder sb;
extern adat<const void*, 32> marked;

class actable {
	gender_s	gender;
public:
	static void	actv(stringbuilder& sb, const char* format, const char* format_param, char separator, const char* name, gender_s gender);
	gender_s	getgender() const { return gender; }
	void		setgender(gender_s v) { gender = v; }
};
bool ismarked(const void* p);
void markvalue(const void* p);