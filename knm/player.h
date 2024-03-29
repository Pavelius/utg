#include "ability.h"
#include "color.h"
#include "entity.h"
#include "upgrade.h"
#include "variant.h"

#pragma once

struct provincei;
struct uniti;

struct playeri : entity, upgradea {
	int			initiative;
	abilitya	current;
	color		fore, border;
	provincei*	homeland;
	uniti*		troops[5];
	abilitya	bonus[5];
	variants	start;
	void		add(ability_s v, int i) { current.abilities[v] += i; }
	int			getarmy(ability_s v) const;
	int			getmaximum(ability_s v) const;
	void		set(ability_s v, int i) { current.abilities[v] = i; }
	void		set(tag_s v) { entity::set(v); }
	void		remove(tag_s v) { entity::remove(v); }
};
extern playeri *player, *speaker;

void prepare_game_ui();