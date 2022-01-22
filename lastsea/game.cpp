#include "main.h"

gamei game;

bool gamei::match(variant v) {
	for(auto& e : bsdata<pirate>()) {
		if(e.match(v))
			return true;
	}
	return false;
}