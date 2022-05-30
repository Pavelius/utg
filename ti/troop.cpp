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
	return add(pu, player, 0);
}

troop* troop::add(const uniti* unit, playeri* player, entity* location) {
	auto p = addnew();
	p->clear();
	p->type = unit;
	p->player = player;
	p->location = location;
	return p;
}

int	troop::getstackcount() const {
	auto pe = bsdata<troop>::end();
	auto result = 1;
	auto v1 = location;
	auto v2 = type;
	for(auto p = this + 1; p < pe; p++) {
		if(p->location == v1 && p->type==v2)
			result++;
	}
	return result;
}