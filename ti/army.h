#include "entitya.h"

#pragma once

struct army {
	playeri*		owner;
	entitya			units;
	char			bonuses[10];
	bool			reatreat;
	static int		round;
	void			clear() { memset(this, 0, sizeof(*this)); }
	void			choose(const char* id);
	void			hit(int value);
	void			hit(int value, tag_s v, bool keep);
	int				roll(abilityn id, abilityn id_count) const;
};
extern army* last_army;