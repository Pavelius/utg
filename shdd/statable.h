#include "ability.h"

#pragma once

struct statable {
	unsigned	weapons, armors, weapon_matery;
	char		abilities[HP + 1];
	void		add(ability_s i, int v) { abilities[i] += v; }
	void		applybest(ability_s v);
	int			get(ability_s i) const { return abilities[i]; }
	static int	getattackbonus(int type, int level);
	ability_s	getbestability() const;
	int			getbonus(ability_s v) const;
	bool		isvalidability() const;
	void		load(const statable& v);
	void		rollability();
};
