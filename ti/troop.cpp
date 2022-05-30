#include "main.h"

static troop* addnew() {
	for(auto& e : bsdata<troop>()) {
		if(!e.type)
			return &e;
	}
	return bsdata<troop>::add();
}

troop* troop::add(const char* id, playeri* player) {
	auto pu = bsdata<uniti>::find(id);
	if(!pu)
		return 0;
	auto p = addnew();
	p->clear();
	p->type = pu;
	p->player = player;
	return p;
}