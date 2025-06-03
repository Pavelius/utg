#include "nameable.h"

#pragma once

enum abilityn : unsigned char {
	Cost, Limit, LimitInProvince, Recruit, Tools,
	Strenght, Combat, Damage, Shoots, Shield, Hirelings, Milita, Move,
	Resources, Influence, Goods,
	Hero, Army, Tactic,
	Fame, Gold,
};
struct abilityi : nameable {
	abilityn	payoff;
};
struct abilitya {
	char		abilities[Gold + 1];
};
extern abilityn last_ability;
