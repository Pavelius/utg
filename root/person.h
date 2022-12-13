#include "crt.h"
#include "harmable.h"
#include "gender.h"
#include "nameable.h"
#include "stringbuilder.h"

#pragma once

struct person : nameable, harmable {
	harmable	inflict;
	gender_s	gender;
	void		actv(stringbuilder& sb, const char* format, const char* param, char add_sep = ' ') const;
	gender_s	getgender() const { return gender; }
};