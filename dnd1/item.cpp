#include "main.h"

static enchantmenti magic_armor_or_shield[] = {
	{},
	{1, 1},
	{2, 2},
	{3, 3},
	{1, -1},
	{2, -2},
	{3, -3},
};
static enchantmenti magic_weapon[] = {
	{},
	{1, 1},
	{4, 2},
	{6, 3},
	{1, -1},
	{4, -2},
	{6, -3},
};

void item::setcount(int v) {
	if(v <= 0)
		clear();
	else if(iscountable())
		count = v - 1;
}

int item::getcount() const {
	if(!type)
		return 0;
	return iscountable() ? count + 1 : 1;
}

void item::add(item& v) {
	if(v.type == type && iscountable()) {
		unsigned n1 = count + v.count;
		if(n1 >= 0xFFFF) {
			count = 0xFFFF;
			v.count = n1 - count;
		} else
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