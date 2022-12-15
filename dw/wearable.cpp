#include "main.h"

void wearable::additem(item& it) {
	for(auto& e : backpack()) {
		if(e)
			continue;
		e = it;
		it.clear();
		break;
	}
}