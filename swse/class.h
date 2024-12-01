#pragma once
#include "skill.h"

enum classn : unsigned char {
	Nonheroic, Beast,
	Jedi, Noble, Scoundrell, Scout, Soldier
};
extern classn last_class;
struct classi : nameable {
	char hd, force, skill;
	skillf skills;
};
struct classable {
	char classes[Soldier + 1];
	int	getlevel() const;
	int	gethlevel() const;
};
