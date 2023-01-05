#include "nameable.h"

#pragma once

enum combat_s : unsigned char {
	Strenght, Damage, Health, Charge, Archery, Level, Armor,
	Movement, Explore, Siege,
	Limit,
};
typedef char combata[Limit + 1];
struct combati : nameable {
};
struct combatable {
	combata combat;
	void	addcombat(combat_s v, int i) { combat[v] += i; }
};