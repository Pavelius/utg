#include "bsdata.h"
#include "gender.h"
#include "flagable.h"
#include "stringbuilder.h"

#pragma once

enum racen : unsigned char {
	Human, Dwarf, Elf, Halfling,
};
typedef flagable<1 + Halfling / 8> racea;
struct racei {
	const char*		id;
};
struct raceable {
	const char*		id;
	racen			race;
	gendern			gender;
public:
	gendern			getgender() const { return gender; }
	const char*		getname() const { return getnm(id); }
	racen			getrace() const { return race; }
};
