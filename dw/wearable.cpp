#include "main.h"

bool wearable::additem(const item& it) {
	for(auto i = Backpack; i <= LastBackpack; i = (wear_s)(i+1)) {
		if(wears[i])
			continue;
		wears[i] = it;
		return true;
	}
	return false;
}