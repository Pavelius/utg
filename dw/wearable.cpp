#include "tag.h"
#include "wearable.h"

void wearable::additem(item& it) {
	for(auto& e : backpack()) {
		if(e)
			continue;
		e = it;
		it.clear();
		break;
	}
}

void wearable::additem(const item& it, int count) {
	if(it.geti().tags.is(Coins))
		coins += it.getcost() * count;
	else {
		while(count--) {
			item itc = it;
			additem(itc);
		}
	}
}

bool wearable::removeitem(int item_type, int count) {
	if(bsdata<itemi>::elements[item_type].tags.is(Coins)) {
		if(coins >= count)
			coins -= count;
		else
			coins = 0;
		return true;
	} else {
		for(auto& e : backpack()) {
			if(e || e.type != item_type)
				continue;
			e.clear();
			return true;
		}
	}
	return false;
}

int wearable::getuses(int item_type) const {
	auto use_count = 0;
	for(auto& e : wears) {
		if(!e)
			continue;
		if(e.type == item_type)
			use_count += e.getuses();
	}
	return use_count;
}