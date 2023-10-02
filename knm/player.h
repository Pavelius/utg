#include "ability.h"
#include "strategy.h"

#pragma once

struct playeri : entity, abilitya {
	short		initiative;
	int			getmaximum(ability_s v) const;
};
extern playeri*	player;