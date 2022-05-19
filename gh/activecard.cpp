#include "main.h"

static activecardi* addnew() {
	for(auto& e : bsdata<activecardi>()) {
		if(!e.card)
			return &e;
	}
	return bsdata<activecardi>::add();
}

void activecardi::clear() {
	memset(this, 0, sizeof(*this));
}

activecardi* activecardi::add(creaturei* target, playercardi* card, action_s type, int bonus, duration_s duration, char uses) {
	auto p = addnew();
	p->clear();
	p->duration = duration;
	p->target = target;
	p->card = card;
	p->type = type;
	p->bonus = bonus;
	p->uses = uses;
	return p;
}

void activecardi::use() {
	if(type != Use)
		return;
	if(uses > 0)
		uses--;
	if(uses == 0)
		discard();
}

void activecardi::discard() {
	if(card && card->owner)
		card->owner->lost.discard(card);
	clear();
}