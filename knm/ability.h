#include "nameable.h"

#pragma once

enum ability_s : unsigned char {
	Cost, Recruit, Limit,
	Strenght, Combat, Damage, Shoots, Shield,
	Resources, Influence, Goods,
	Tactic, Army,
	Lore, Fame, Gold,
};
struct abilityi : nameable {
};
struct abilitya {
	char		abilities[Gold + 1];
	void		add(ability_s v, int i) { abilities[v] += i; }
	int			get(ability_s v) const { return abilities[v]; }
};
