#include "enumerator.h"

static void auto_increment(enumi* pb, enumi* pe, variant type, int index) {
	while(pb < pe) {
		if(pb->type == type)
			pb->index = index++;
		pb++;
	}
}

void enumi::initialize() {
	auto pe = bsdata<enumi>::end();
	for(auto& e : bsdata<enumi>()) {
		if(e.index)
			continue;
		auto_increment(&e + 1, pe, e.type, 1);
	}
}