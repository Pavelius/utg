#include "harmable.h"
#include "gender.h"
#include "nameable.h"
#include "move.h"
#include "stringbuilder.h"

#pragma once

extern stringbuilder console;

struct actable : nameable, harmable {
	harmable	inflict;
	gendern		gender;
	void		actv(stringbuilder& sb, const char* format, const char* param, char add_sep = ' ') const;
	void		act(const char* format, ...) { actv(console, format, xva_start(format)); }
	gendern		getgender() const { return gender; }
};