#include "ability.h"
#include "card.h"
#include "entitya.h"
#include "player.h"
#include "troop.h"
#include "unit.h"

entitya querry;

static int compare_units(const void* v1, const void* v2) {
	auto e1 = *((entity**)v1);
	auto e2 = *((entity**)v2);
	auto p1 = e1->getunit();
	auto p2 = e2->getunit();
	if(e1->player != e2->player)
		return e1->player - e2->player;
	if(p1->abilities[Cost] != p2->abilities[Cost])
		return p1->abilities[Cost] - p2->abilities[Cost];
	if(p1->abilities[Army] != p2->abilities[Army])
		return p2->abilities[Army] - p2->abilities[Army];
	return e1 - e2;
}

void entitya::sortunits() {
	sort(compare_units);
}

int	entitya::gettotal(abilityn v) const {
	auto result = 0;
	for(auto p : *this)
		result += p->get(v);
	return result;
}

int	entitya::getarmy() const {
	return (gettotal(Army) + 9) / 10;
}

int	entitya::gettotal(const entity* type) const {
	auto result = 0;
	for(auto p : *this) {
		if((entity*)p->id == type)
			result++;
	}
	return result;
}

int	entitya::getplayercount() const {
	playeri* nv = 0;
	auto result = 0;
	for(auto p : *this) {
		if(p->player != nv) {
			result++;
			nv = p->player;
		}
	}
	return result;
}

void entitya::select(const entity* location) {
	auto ps = data;
	auto pe = endof();
	for(auto& e : bsdata<troopi>()) {
		if(e.location != location)
			continue;
		if(ps >= pe)
			break;
		*ps++ = &e;
	}
	count = ps - data;
}

void entitya::select(const entity* location, const entity* player) {
	auto ps = data;
	auto pe = endof();
	for(auto& e : bsdata<troopi>()) {
		if(e.location != location || e.player != player)
			continue;
		if(ps >= pe)
			break;
		*ps++ = &e;
	}
	count = ps - data;
}

void entitya::select(const entity* deck, const entity* location, const entity* player) {
	auto ps = data;
	auto pe = endof();
	for(auto& e : bsdata<card>()) {
		if(!e)
			continue;
		if(e.player != player || e.location != location)
			continue;
		if(e.getcomponent()->location != deck)
			continue;
		if(ps >= pe)
			break;
		*ps++ = &e;
	}
	count = ps - data;
}