#include "tag.h"
#include "wearable.h"

void wearable::additem(item& it) {
	if(it.geti().tags.is(Coins)) {
		coins += it.getcost();
		it.clear();
		return;
	}
	for(auto& e : backpack()) {
		if(e)
			continue;
		e = it;
		it.clear();
		break;
	}
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