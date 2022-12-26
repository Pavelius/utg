#include "action.h"
#include "flagable.h"

#pragma once

enum tag_s : unsigned char {
	Absorb, Armor, Bulky, CloseRanks, Clumsy, Deadly, GentleThrashing, Handy,
	Fast, Fragile, Heavy, Hooked, LongRange,
	MediumRange, Missile, Protection, ShortAndQuick, Slow,
	Thrown, TwoHanded, HeavySkillPenalty, Useful, Unwieldy,
	Versatile
};
typedef flagable<(7 + Versatile) / 8> taga;
struct tagi {
	const char*		id;
	actiona			bonus;
	actiona			success;
};
