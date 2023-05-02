#include "nameable.h"

#pragma once

enum race_s : unsigned char {
	NoRace,
	Human, Dwarf, Elf, HalfOrc,
};

struct racei : nameable {
};
