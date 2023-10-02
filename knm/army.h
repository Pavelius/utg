#include "ability.h"
#include "entitya.h"

#pragma once

struct armyi : abilitya {
	entitya		troops;
	void		add(ability_s v, int i) { abilitya::add(v, i); }
	void		clear();
	void		damage(ability_s type, int chance, int count = 1, int chance_critical = 1);
	void		engage(armyi& enemy, ability_s attack_type, ability_s damage_type);
	void		engage(armyi& enemy);
	int			getcount(entity* type) const;
	void		sort();
};
extern armyi attacker, defender;