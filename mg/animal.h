#include "gender.h"
#include "skill.h"

#pragma once

struct animali : nameable {
	gender_s		gender;
	char			nature;
	skilla			skills;
};
void create_animal(const char* id);
