#include "bsdata.h"
#include "gender.h"
#include "flagable.h"
#include "stringbuilder.h"

#pragma once

enum racen : unsigned char {
	Human, Dwarf, Elf, Halfling,
};
typedef flagable<1, unsigned char> racea;
struct racei {
	const char*	id;
};
struct raceable {
	racen		race;
	gendern		gender;
};
