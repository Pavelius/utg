#include "ability.h"
#include "entitya.h"

#pragma once

struct armyi : abilitya {
	entitya		troops;
	entitya		casualty;
	void		add(ability_s v, int i) { abilities[v] += i; }
	void		applycasualty();
	void		clear();
	void		damage(int chance, int count = 1);
	void		engage(ability_s type, int skill = 0);
	int			get(ability_s v) const { return abilities[v]; }
	void		prepare(ability_s type);
	void		resist(int& hits);
	void		suffer(int hits);
};
extern armyi	attacker, defender;
extern armyi*	last_army;