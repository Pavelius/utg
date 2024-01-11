#include "item.h"

int	item::getcostall() const {
	auto cost = getcost() * geti().count;
	auto default_count = geti().count;
	if(default_count)
		cost = cost / default_count;
	return cost;
}

void item::add(item& v) {
	if(type != v.type || data != v.data)
		return;
	auto new_count = count + (1 + v.count);
	if(new_count <= 255) {
		v.count = 0;
		count = new_count;
	} else {
		v.count = new_count - 255;
		count = 255;
	}
}

bool item::canequip(wear_s v) const {
	switch(v) {
	case LeftFinger:
	case RightFinger:
		return geti().wear == LeftFinger;
	default:
		return geti().wear == v;
	}
}