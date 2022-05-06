#include "main.h"

bool cardpool::isdoubleclue(ability_s v) const {
	for(auto& e : source) {
		if(!e.isactive())
			continue;
		if(e.geti().isdoubleclue(v))
			return true;
	}
	return false;
}

bool cardpool::isrerollall(ability_s v) const {
	for(auto& e : source) {
		if(!e.isactive())
			continue;
		if(e.geti().isrerollall(v))
			return true;
	}
	return false;
}

void cardpool::discard() {
	for(auto& e : source) {
		if(e)
			e.discard();
	}
}

void cardi::clear() {
	memset(this, 0, sizeof(*this));
}

void cardi::discard() {
	bsdata<cardtypei>::elements[geti().type].cards.drop(type);
	clear();
}

int	cardprotoi::getcost(int discount) const {
	int v = cost + discount;
	if(v < 0)
		v = 0;
	return v;
}

void cardpool::addcard(cardt v) {
	auto p = source.add();
	p->clear();
	p->type = v;
}

bool cardpool::havecard(cardt v) const {
	for(auto& e : source) {
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
		addcard(i);
}