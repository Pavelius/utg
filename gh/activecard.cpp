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

activecardi* activecardi::add(creaturei* target, playercardi* card, duration_s duration, char uses, const slice<variant>& source) {
	auto p = addnew();
	p->clear();
	p->duration = duration;
	p->target = target;
	p->card = card;
	p->uses = uses;
	p->source = source;
	return p;
}

void activecardi::use() {
	if(duration != Use)
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