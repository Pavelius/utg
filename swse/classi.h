#pragma once
#include "skill.h"

enum class_s : unsigned char {
	Nonheroic, Beast,
	Jedi, Noble, Scoundrell, Scout, Soldier
};

struct classi : nameable {
	char	hd, force, skill;
	skilla	skills;
};

struct classable {
	char	classes[Soldier + 1];
};
