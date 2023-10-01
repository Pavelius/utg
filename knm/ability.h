#include "nameable.h"

#pragma once

enum ability_s : unsigned char {
	Cost,
	Strenght, Melee, Shoot, Messy,
	MeleeDefence, ShootDefence,
	Recruit, Reinforcement,
	Resources, Influence, Trade,
	Champion, General, Hero,
	Lore, Fame, Goods,
};
struct abilityi : nameable {
};
struct abilitya {
	char		abilities[Goods + 1];
	void		add(ability_s v, int i) { abilities[v] += i; }
	int			get(ability_s v) const { return abilities[v]; }
};
