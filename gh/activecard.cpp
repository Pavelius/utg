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

void activecarda::apply(variants source) {
	for(auto v : source) {
		if(v.iskind<actioni>())
			actions[v.value] += v.counter;
	}
}