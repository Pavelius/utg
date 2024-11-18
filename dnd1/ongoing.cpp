#include "actable.h"
#include "calendar.h"
#include "ongoing.h"

static void act(spelln effect, const char* action) {
	printa(bsdata<spelli>::elements[effect].id, action);
}

static ongoing* find_bonus(variant owner, spelln effect) {
	for(auto& e : bsdata<ongoing>()) {
		if(e.owner == owner && e.effect == effect)
			return &e;
	}
	return 0;
}

void enchant(variant caster, variant owner, spelln effect, unsigned rounds) {
	rounds += current_round;
	auto p = find_bonus(owner, effect);
	if(!p) {
		p = bsdata<ongoing>::add();
		p->owner = owner;
		p->caster = caster;
		p->effect = effect;
		p->rounds = rounds;
	} else if(p->rounds < rounds)
		p->rounds = rounds;
}

void dispell(variant owner, spelln effect) {
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