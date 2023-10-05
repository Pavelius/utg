#include "ability.h"
#include "color.h"
#include "strategy.h"

#pragma once

struct provincei;

struct playeri : entity {
	int			initiative;
	abilitya	current;
	color		fore, border;
	provincei*	homeland;
	int			getmaximum(ability_s v) const;
	int			getsummary(ability_s v) const;
};
extern playeri*	player;

void prepare_game_ui();