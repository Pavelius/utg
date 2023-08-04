#include "entitya.h"

#pragma once

struct army {
	playeri*		owner;
	entitya			units;
	char			bonuses[10];
	bool			reatreat;
	static army*	last;
	static int		round;
	void			clear() { memset(this, 0, sizeof(*this)); }
	void			choose(const char* id);
	void			hit(int value);
	int				roll(ability_s id, ability_s id_count) const;
};