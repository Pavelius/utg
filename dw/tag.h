#pragma once

#include "flagable.h"
#include "nameable.h"

enum tagn : unsigned char {
	Arrows, CharismaMinus, Clumsy, Dangerous, Forceful, IgnoreArmor, Messy, NeedArrows, Precise,
	Ration, Reload, Slow, Stun, Thrown, TwoHanded,
	Coins,
	Injured, Shaken, Sick, Muddled, Tired, Stressed,
	DivineShock,
	HealDebility, HealPoison, UsableGear,
	Advantage, Disadvantage,
	Hand, Close, Reach, Near, Far,
};
struct tagi : nameable {
	unsigned char	ignore;
};
typedef flagable<1, unsigned long> taga;

