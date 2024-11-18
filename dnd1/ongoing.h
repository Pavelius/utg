#include "spell.h"
#include "variant.h"

#pragma once

struct ongoing {
	variant		owner;
	variant		caster;
	spelln		effect;
	unsigned	rounds;
	void		clear() { memset(this, 0, sizeof(*this)); }
};
void dispell(variant onwer, spelln effect);
void enchant(variant caster, variant owner, spelln effect, unsigned rounds);