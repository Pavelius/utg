#include "item.h"

item* last_item;
itemi* item_armor;
itemi* item_weapon;
itemad items;

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
	if(!count)
		count = 1;
	create(pi - bsdata<itemi>::elements, pi->count ? pi->count * count : count);
}

void item::create(unsigned short p_type, int p_count) {
	if(!p_count)
		p_count = 1;
	clear();
	this->type = p_type;
	setcount(p_count);
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
	last_item = this;
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

variant item::getpower() const {
	auto& ei = geti();
	if(!ei.powers || !count)
		return variant();
	return ei.powers->elements.begin()[count - 1];
}

void item::damage() {
	if(damaged >= 3)
		clear();
	else
		damaged++;
}