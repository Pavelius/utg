#include "ability.h"
#include "entitya.h"
#include "troop.h"

entitya querry;

int	entitya::gettotal(ability_s v) const {
	auto result = 0;
	for(auto p : *this)
		result += p->get(v);
	return result;
}

int	entitya::getarmy() const {
	return (gettotal(Army) + 9) / 10;
}

void entitya::select(const entity* object) {
	auto ps = data;
	auto pe = endof();
	for(auto& e : bsdata<troopi>()) {
		if(e.location != object)
			continue;
		if(ps >= pe)
			break;
		*ps++ = &e;
	}
	count = ps - data;
}