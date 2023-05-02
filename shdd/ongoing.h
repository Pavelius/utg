#include "variant.h"

#pragma once

struct ongoing {
	variant			owner;
	variant			effect;
	unsigned		stop_round;
	static void		add(variant owner, variant effect, unsigned stop_round);
	static ongoing*	find(variant owner, variant effect);
	void			clear() { memset(this, 0, sizeof(*this)); }
};
