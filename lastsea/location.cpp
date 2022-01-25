#include "main.h"

int locationi::getpriority(variant v) const {
	auto i = 0;
	for(auto e : actions) {
		if(e == v)
			return i;
		i++;
	}
	return i;
}

const locationi* locationi::find(short unsigned scene) {
	for(auto& e : bsdata<locationi>()) {
		if(e.scene == scene)
			return &e;
	}
	return 0;
}
