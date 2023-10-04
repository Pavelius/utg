#include "ability.h"
#include "color.h"
#include "strategy.h"

#pragma once

struct playeri : entity {
	int			initiative;
	abilitya	current;
	color		fore, border;
	int			getmaximum(ability_s v) const;
	int			getprovincesummary(ability_s v) const;
};
extern playeri*	player;