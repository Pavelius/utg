#include "ability.h"

#pragma once

enum class_s : unsigned char {
	Monster,
	Cleric, Dwarf, Elf, Fighter, Halfling, Theif, Wizard,
};
struct classi : nameable {
	ability_s	prime;
	char		minimal[6];
	int			tohit, hd;
};
