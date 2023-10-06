#include "nameable.h"

#pragma once

enum ability_s : unsigned char {
	Cost, Limit,
	Strenght, Combat, Damage, Shoots, Shield,
	Resources, Influence, Goods,
	Tactic, Army,
	Lore, Fame, Gold,
};
struct abilityi : nameable {
};
struct abilitya {
	char		abilities[Gold + 1];
};
