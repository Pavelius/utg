#include "flagable.h"

#pragma once

struct statable {
	char		abilitites[32];
	flagable<2>	saves;
	flagable<4>	skills;
	flagable<8>	items;
	flagable<2>	resist, immunity, vulnerable;
	flagable<4>	tags;
	int			getbonus(int v) const { return abilitites[v] / 2 - 5; }
	void		load(const statable& e) { *this = e; }
};