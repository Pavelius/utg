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

activecardi* activecardi::add(playeri* player, playercardi* card, variants effect) {
	auto p = addnew();
	p->clear();
	p->player = player;
	p->card = card;
	p->effect = effect;
	p->type = game.getduration(p->effect);
	if(p->type == Use) {
		p->uses = game.getrounds(p->effect);
		p->uses_experience = p->uses;
	}
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