#pragma once
#include "skill.h"

enum classn : unsigned char {
	Nonheroic, Beast,
	Jedi, Noble, Scoundrell, Scout, Soldier
};
//enum classn : unsigned char;
extern classn last_class;
struct classi : nameable {
	char hd, force, skill;
	skillf skills;
};
struct classable {
	char classes[16];
	int	getlevel() const;
	int	gethlevel() const;
};
