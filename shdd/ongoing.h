#include "spell.h"
#include "variant.h"

#pragma once

struct ongoing {
	variant		owner;
	spell_s		effect;
	unsigned	rounds;
	void		clear() { memset(this, 0, sizeof(*this)); }
};
