#include "calendar.h"
#include "ongoing.h"
#include "print.h"

static void act(spell_s effect, const char* action) {
	printa(bsdata<spelli>::elements[effect].id, action);
}

static ongoing* find_bonus(variant owner, spell_s effect) {
	for(auto& e : bsdata<ongoing>()) {
		if(e.owner == owner && e.effect == effect)
			return &e;
	}
	return 0;
}

void enchant(variant owner, spell_s effect, unsigned rounds) {
	rounds += current_round;
	auto p = find_bonus(owner, effect);
	if(!p) {
		p = bsdata<ongoing>::add();
		p->owner = owner;
		p->effect = effect;
		p->rounds = rounds;
	} else if(p->rounds < rounds)
		p->rounds = rounds;
}

void dispell(variant owner, spell_s effect) {
	for(auto& e : bsdata<ongoing>()) {
		if(e.effect == effect && e.owner == owner) {
			act(e.effect, "Dispell");
			e.clear();
		}
	}
}

void dispell(variant owner) {
	for(auto& e : bsdata<ongoing>()) {
		if(e.owner == owner) {
			act(e.effect, "Dispell");
			e.clear();
		}
	}
}