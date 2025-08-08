#pragma once

#include "ability.h"
#include "flagable.h"

struct statable {
	char	abilitites[Focus];
	flag32	pickextra, scavenge;
	flag32	tought, restore, reroll;
};
