#include "bsdata.h"
#include "gender.h"
#include "flagable.h"
#include "stringbuilder.h"

#pragma once

enum race_s : unsigned char {
	Human, Dwarf, Elf, Halfling,
};
typedef flagable<1 + Halfling / 8> racea;
struct racei {
	const char*		id;
};
struct raceable {
	const char*		id;
	race_s			race;
	gender_s		gender;
public:
	gender_s		getgender() const { return gender; }
	const char*		getname() const { return getnm(id); }
	race_s			getrace() const { return race; }
};
