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