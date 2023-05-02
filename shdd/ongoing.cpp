#include "ongoing.h"

BSDATAC(ongoing, 512)

void ongoing::add(variant owner, variant effect, unsigned stop_round) {
	auto p = find(owner, effect);
	if(!p) {
		p = bsdata<ongoing>::add();
		p->owner = owner;
		p->effect = owner;
	}
	p->stop_round = stop_round;
}

ongoing* ongoing::find(variant owner, variant effect) {
	for(auto& e : bsdata<ongoing>()) {
		if(e.owner == owner && e.effect == effect)
			return &e;
	}
	return 0;
}