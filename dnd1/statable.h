#include "ability.h"
#include "class.h"
#include "feat.h"

#pragma once

struct statable {
	char			abilities[IllusionCopies + 1];
	featable		feats;
	void			add(ability_s i, int v) { abilities[i] += v; }
	void			applybest(ability_s v);
	void			apply(const classi* pi);
	int				get(ability_s i) const { return abilities[i]; }
	static int		getattackbonus(int type, int level);
	ability_s		getbestability() const;
	int				getbonus(ability_s v) const;
	int				getbonush(ability_s v) const;
	char			getspells(int spell_level, int level);
	void			rollability();
};
