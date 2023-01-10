#include "nameable.h"

#pragma once

enum cost_s : unsigned char {
	Gold, Mana, Happiness, Fame, Trade, Lore, Warfire,
	Strenght, Damage, Health, Charge, Archery, Level, Armor,
	Movement, Explore, ExploreNext, Siege, Size,
	Limit,
};
struct costi : nameable {
	int			width;
	const char*	format;
};
typedef short costa[Limit + 1];
void addvalue(costa& v1, const costa& v2);
bool isenought(const costa& v1, const costa& v2);
void subvalue(costa& v1, const costa& v2);