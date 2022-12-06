#pragma once
#include "flagable.h"

struct statable {
	char		abilitites[32];
	flagable<2>	saves;
	flagable<4>	skills;
	flagable<8>	items;
	flagable<2>	resist, immunity, vulnerable;
	flagable<4>	tags;
	void		load(const statable& e) { *this = e; }
};
