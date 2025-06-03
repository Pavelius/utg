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

void activecardi::addsource(variant* p, variant* pe) {
	while(p < pe) {
		if(p->iskind<actioni>()) {
			if(p->type == Discard)
				discard_action = (actionn)p->type;
			else {
				type = (actionn)p->type;
				modifiera modifiers = {};
				p = creaturei::getmodifiers(p + 1, pe, modifiers);
				bonus = modifiers[Bonus];
			}
		} else
			break;
	}
}

activecardi* activecardi::add(creaturei* target, playercardi* card, duration_s duration, char uses, const slice<variant>& source) {
	auto p = addnew();
	p->clear();
	p->duration = duration;
	p->target = target;
	p->card = card;
	p->uses = uses;
	p->addsource(source.begin(), source.end());
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
	if(card && card->owner) {
		switch(discard_action) {
		case Discard:
			card->owner->lost.discard(card);
			break;
		}
	}
	clear();
}