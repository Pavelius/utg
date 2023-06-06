#include "nameable.h"
#include "flagable.h"

#pragma once

enum feat_s : unsigned char {
	Coins, Ambush, BonusTalent, BonusHitRoll,
	Finesse, Loading, Thrown, TwoHanded, Versatile,
	Summoned, Player, Enemy,
};
struct feati : nameable {
};
struct featable : flagable<4> {};