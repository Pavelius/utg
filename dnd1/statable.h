#include "ability.h"
#include "class.h"
#include "feat.h"

#pragma once

struct statable {
	char			abilities[IllusionCopies + 1];
	featable		feats;
	void			add(abilityn i, int v) { abilities[i] += v; }
	void			applybest(abilityn v);
	void			apply(const classi* pi);
	int				get(abilityn i) const { return abilities[i]; }
	static int		getattackbonus(int type, int level);
	abilityn		getbestability() const;
	int				getbonus(abilityn v) const;
	int				getbonush(abilityn v) const;
	char			getspells(int spell_level, int level);
	void			rollability();
};
