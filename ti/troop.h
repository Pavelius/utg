#include "entity.h"

#pragma once

struct troop : entity {
	flagable<1>		flags;
	void			clear() { memset(this, 0, sizeof(*this)); }
	void			hit();
	void			paint() const;
	void			produce(const uniti* unit) const;
	static void		updateui();
};
extern troop* last_troop;

troop* create_troop(const char* id, playeri* player);
troop* create_troop(const uniti* unit, playeri* player, entity* location);