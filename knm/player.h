#include "ability.h"
#include "color.h"
#include "strategy.h"

#pragma once

struct provincei;
struct uniti;

struct playeri : entity {
	int			initiative;
	abilitya	current;
	color		fore, border;
	provincei*	homeland;
	uniti*		troops[5];
	int			getmaximum(ability_s v) const;
	int			getsummary(ability_s v) const;
	void		set(ability_s v, int i) { current.abilities[v] = i; }
	void		set(tag_s v) { entity::set(v); }
};
extern playeri*	player;

void prepare_game_ui();