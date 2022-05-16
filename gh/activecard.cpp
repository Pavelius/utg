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
		p->uses = 3;
		p->uses_experience = 3;
	}
	return p;
}