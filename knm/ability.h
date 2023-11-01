#include "nameable.h"

#pragma once

enum ability_s : unsigned char {
	Cost, Limit, LimitInProvince, Recruit, Tools,
	Strenght, Combat, Damage, Shoots, Shield, Hirelings, Milita, Move,
	Resources, Influence, Goods,
	Hero, Army, Tactic,
	Fame, Gold,
};
struct abilityi : nameable {
	ability_s	payoff;
};
struct abilitya {
	char		abilities[Gold + 1];
};
extern ability_s last_ability;
