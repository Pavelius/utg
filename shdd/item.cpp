#include "item.h"

void itema::select(const slice<item>& source) {
	for(auto& e : source) {
		if(e)
			add(&e);
	}
}

void itema::match(wear_s wear, bool keep) {
	auto ps = begin();
	for(auto p : *this) {
		if((p->geti().wear == wear) != keep)
			continue;
		*ps++ = p;
	}
	count = ps - begin();
}

void item::create(const char* id, int count) {
	create(bsdata<itemi>::find(id), count);
}

void item::create(const itemi* pi, int count) {
	if(!pi)
		return;
	create(pi - bsdata<itemi>::elements, pi->count ? pi->count * count : count);
}

void item::create(unsigned short type, int count) {
	clear();
	this->type = type;
	setcount(count);
}

int item::getcount() const {
	if(iscountable())
		return count + 1;
	return 1;
}

void item::setcount(int v) {
	if(v <= 0)
		clear();
	else if(iscountable())
		count = v - 1;
}

void item::add(item& v) {
	if(type != v.type || flags != v.flags)
		return;
	if(iscountable())
		return;
	unsigned n1 = count + v.count + 1;
	if(n1 >= 0xFF) {
		count = 0xFF;
		v.count = n1 - count - 1;
	} else {
		count = n1;
		v.clear();
	}
}

bool item::canequip(wear_s v) const {
	auto w = geti().wear;
	switch(w) {
	case FingerRight: case FingerLeft:
		return v == FingerLeft || v == FingerRight;
	default:
		return v == w;
	}
}

void item::addname(stringbuilder& sb) const {
	auto count = getcount();
	auto name = getnm(geti().id);
	sb.add(name);
	if(count > 1)
		sb.adds("%1i%Pieces", count);
}

dice item::getdamage() const {
	return dice{1, (char)geti().damage};
}

int item::getcost() const {
	auto& ei = geti();
	return getcount() * ei.cost / (ei.count ? ei.count : 1);
}

const itempoweri* item::getpower() const {
	auto& ei = geti();
	if(!ei.powers[0] || !power)
		return 0;
	return ei.powers[power - 1];
}