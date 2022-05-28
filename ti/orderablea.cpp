#include "main.h"

int orderablea::fight(ability_s power, ability_s count) {
	auto result = 0;
	for(auto p : *this) {
		if(!p)
			continue;
		result += p->fight(p->get(power), p->get(count));
	}
	return result;
}

void orderablea::select(const playeri* player, const orderable* location) {
	auto ps = data;
	auto pe = endof();
	for(auto& e : bsdata<troop>()) {
		if(!e)
			continue;
		if(e.player != player || e.location != location)
			continue;
		if(ps < pe)
			*ps++ = &e;
	}
	count = ps - data;
}

void orderablea::select(array& source) {
	auto ps = data;
	auto px = endof();
	auto pe = source.end();
	auto size = source.getsize();
	for(auto pb = source.begin(); pb < pe; pb += size) {
		auto p = (orderable*)pb;
		if(!(*p))
			continue;
		if(ps < px)
			*ps++ = p;
	}
	count = ps - data;
}