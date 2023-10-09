#include "nameable.h"

#pragma once

enum ability_s : unsigned char {
	Cost, Limit, LimitInProvince, Recruit,
	Strenght, Combat, Damage, Shoots, Shield, Move,
	Resources, Influence, Goods,
	Hero, Army,
	Lore, Fame, Gold,
};
struct abilityi : nameable {
};
struct abilitya {
	char		abilities[Gold + 1];
};
extern ability_s last_ability;
