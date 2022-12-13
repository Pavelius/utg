#include "ability.h"

#pragma once

struct statable {
	char abilities[Might + 1];
	int	get(ability_s v) const { return abilities[v]; }
};