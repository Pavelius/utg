#include "main.h"

int actioni::getstage(int v) const {
	if(v < 1)
		v = 1;
	auto r = 0;
	for(auto minimum : result) {
		if(!minimum)
			break;
		if(v >= minimum)
			r++;
	}
	return r;
}

variants actioni::getoutcome(int v) const {
	switch(v) {
	case 1: return outcome1;
	case 2: return outcome2;
	case 3: return outcome3;
	case 4: return outcome4;
	case 5: return outcome5;
	case 6: return outcome6;
	default: return {};
	}
}