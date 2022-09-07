#include "main.h"

void item::create(const char* id, int count) {
	create(bsdata<itemi>::find(id), count);
}

void item::create(const itemi* pi, int count) {
	if(!pi)
		return;
	clear();
	type = pi - bsdata<itemi>::elements;
	if(pi->count)
		setcount(count * pi->count);
	else
		setcount(count);
}

void item::setcount(int v) {
	if(v <= 0)
		clear();
	else if(iscountable())
		count = v - 1;
	else
		count_nocountable = v - 1;
}

int item::getcount() const {
	if(!type)
		return 0;
	return iscountable() ? count + 1 : count_nocountable + 1;
}

void item::add(item& v) {
	if(type != v.type || subtype != v.subtype)
		return;
	if(iscountable()) {
		unsigned n1 = count + v.count + 1;
		if(n1 >= 0xFFFF) {
			count = 0xFFFF;
			v.count = n1 - count - 1;
		} else {
			count = n1;
			v.clear();
		}
	} else {
		unsigned n1 = count_nocountable + v.count_nocountable + 1;
		if(n1 >= 0xFF) {
			count_nocountable = 0xFF;
			v.count_nocountable = n1 - count_nocountable - 1;
		} else {
			count_nocountable = n1;
			v.clear();
		}
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
	auto& r = geti().weapon.damage;
	return r ? r : dice{1, 2};
}

int	item::getweight() const {
	auto& ei = geti();
	return getcount() * ei.weight;
}

int item::getcost() const {
	auto& ei = geti();
	return getcount() * ei.cost / (ei.count ? ei.count : 1);
}

const enchantmenti* item::getenchant() const {
	if(!subtype)
		return 0;
	auto& ei = geti();
	if(!ei.enchantments)
		return 0;
	return ei.enchantments->elements.begin() + subtype - 1;
}