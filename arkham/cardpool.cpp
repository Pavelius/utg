#include "main.h"

void cardpool::discard() {
	for(auto& e : *this) {
		if(e)
			e.discard();
	}
}

void cardi::clear() {
	game.unequip(this);
	memset(this, 0, sizeof(*this));
}

void cardi::discard() {
	bsdata<cardtypei>::elements[geti().type].cards.drop(type);
	clear();
}

void cardi::use() {
	auto& ei = geti();
	if(ei.is(Exhause))
		exhaused = 1;
	else if(ei.is(Discard))
		discard();
}

void cardpool::add(cardt v, location_s a) {
	auto p = adat<cardi>::add();
	p->clear();
	p->type = v;
	p->area = a;
}

bool cardpool::have(cardt v) const {
	for(auto& e : *this) {
		if(!e)
			continue;
		if(e.type == v)
			return true;
	}
	return false;
}

void cardpool::pick(cardtype_s type, int count) {
	while(count-- > 0)
		pick(type);
}

void cardpool::pick(cardtype_s type) {
	auto i = bsdata<cardtypei>::elements[type].cards.pick();
	if(i)
		add(i);
}