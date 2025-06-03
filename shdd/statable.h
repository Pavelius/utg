#include "ability.h"
#include "state.h"

#pragma once

struct statable {
	unsigned	usable, mastery;
	stateable	states;
	char		abilities[Dodge + 1];
	void		add(abilityn i, int v) { abilities[i] += v; }
	void		applybest(abilityn v);
	int			get(abilityn i) const { return abilities[i]; }
	static int	getattackbonus(int type, int level);
	abilityn	getbestability() const;
	int			getbonus(abilityn v) const;
	bool		isvalidability() const;
	void		load(const statable& v);
	void		rollability();
};
