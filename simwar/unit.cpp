#include "unit.h"

uniti* lastunit;

troop* find_troop(const uniti* type, const provincei* province) {
	for(auto& e : bsdata<troop>()) {
		if(e.province == province && e.type == type)
			return &e;
	}
	return 0;
}

troop* find_troop(const uniti* type, const provincei* province, const playeri* player) {
	for(auto& e : bsdata<troop>()) {
		if(e.moveto == province && e.type == type && e.player == player)
			return &e;
	}
	return 0;
}

const char* troop::getname() const {
	if(!type)
		return "None";
	return str("%1 (%2)", type->getname(), province->getname());
}