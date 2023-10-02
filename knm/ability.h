#include "nameable.h"

#pragma once

enum ability_s : unsigned char {
	Cost, Recruit, Limit,
	Strenght, Combat,
	LightDamage, HeavyDamage, CriticalDamage, Shoots,
	Resources, Influence, Trade,
	Tactic, Army, Strategy,
	Lore, Fame, Goods,
};
struct abilityi : nameable {
};
struct abilitya {
	char		abilities[Goods + 1];
	void		add(ability_s v, int i) { abilities[v] += i; }
	int			get(ability_s v) const { return abilities[v]; }
};
