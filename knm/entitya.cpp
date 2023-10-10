#include "ability.h"
#include "card.h"
#include "entitya.h"
#include "player.h"
#include "troop.h"
#include "unit.h"

entitya querry;

static int compare_units(const void* v1, const void* v2) {
	auto p1 = (*((entity**)v1))->getunit();
	auto p2 = (*((entity**)v2))->getunit();
	if(p1->player != p2->player)
		return p1->player - p2->player;
	if(p1->abilities[Cost] != p2->abilities[Cost])
		return p1->abilities[Cost] - p2->abilities[Cost];
	if(p1->abilities[Army] != p2->abilities[Army])
		return p2->abilities[Army] - p2->abilities[Army];
	return p1 - p2;
}

void entitya::sortunits() {
	sort(compare_units);
}

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