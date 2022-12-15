#include "flagable.h"

#pragma once

enum tag_s : unsigned char {
	Arrows, CharismaMinus, Clumsy, Dangerous, Forceful, IgnoreArmor, Messy, NeedArrows, Precise, Ration, Reload, Slow, Stun, Thrown, TwoHanded,
	Coins, HealDebility, HealPoison,
	UsableGear,
	Hand, Close, Reach, Near, Far,
};
struct tagi {
	const char*		id;
};
typedef flagable<1 + Far / 8> taga;

