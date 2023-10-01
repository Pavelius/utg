#include "ability.h"
#include "collection.h"
#include "troop.h"

#pragma once

struct armyi : abilitya {
	collection<troopi> units;
	void		add(ability_s v, int i) { abilitya::add(v, i); }
	void		clear();
	void		damage(ability_s type, int chance);
	void		engage(armyi& enemy);
};
extern armyi attacker, defender;