#include "spell.h"
#include "variant.h"

#pragma once

struct ongoing {
	variant		owner;
	variant		caster;
	spell_s		effect;
	unsigned	rounds;
	void		clear() { memset(this, 0, sizeof(*this)); }
};
void dispell(variant onwer, spell_s effect);
void enchant(variant caster, variant owner, spell_s effect, unsigned rounds);