#include "nameable.h"
#include "flagable.h"

#pragma once

enum feat_s : unsigned char {
	BonusTalent, BonusHitRoll,
	Summoned, Player, Enemy,
};
struct feati : nameable {
};
struct featable : flagable<4> {};