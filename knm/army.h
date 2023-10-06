#include "ability.h"
#include "entitya.h"

#pragma once

struct armyi : abilitya {
	entitya		troops;
	entitya		casualty;
	void		add(ability_s v, int i) { abilities[v] += i; }
	void		clear();
	void		damage(ability_s type, int chance, int count = 1);
	void		engage();
	void		engage(ability_s type);
	int			get(ability_s v) const { return abilities[v]; }
	void		prepare(ability_s type);
	void		resist(int& hits);
	void		sort();
	void		suffer(int hits);
};
extern armyi attacker, defender;