#include "dice.h"
#include "nameable.h"
#include "variant.h"

#pragma once

struct spelli : nameable {
	int			duration; // 0 - Instant, -1 - for Focus
	dice		damage, heal;
	variants	enchant, summon;
};
