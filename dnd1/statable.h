#include "ability.h"
#include "class.h"
#include "feat.h"

#pragma once

struct statable {
	char			abilities[IllusionCopies + 1];
	featable		feats;
	void			add(ability_s i, int v) { abilities[i] += v; }
	void			applybest(ability_s v);
	void			applyminimal(class_s v);
	int				get(ability_s i) const { return abilities[i]; }
	static int		getattackbonus(int type, int level);
	ability_s		getbestability() const;
	int				getbonus(ability_s v) const;
	int				getbonush(ability_s v) const;
	static unsigned	getexperience(class_s type, int level);
	char			getspells(class_s type, int spell_level, int level);
	void			rollability();
};
