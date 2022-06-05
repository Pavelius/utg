#include "main.h"

systemi* systemi::last;
systemi* systemi::active;

bool systemi::isactivated(const playeri* p) const {
	auto i = bsdata<playeri>::source.indexof(p);
	if(i == -1)
		return false;
	return activated.is(i);
}

void systemi::setactivate(const playeri* p, bool active) {
	auto i = bsdata<playeri>::source.indexof(p);
	if(i == -1)
		return;
	activated.set(i, active);
}

planeti* systemi::getbestplanet() const {
	entitya planets;
	planets.selectplanets(this);
	return (planeti*)planets.getbest(Resources);
}

void systemi::placement(const uniti* unit, playeri* player) {
	if(unit->type == Ships)
		troop::add(unit, player, this);
	else {
		auto planet = getbestplanet();
		if(planet)
			troop::add(unit, player, planet);
	}
}

void systemi::placement(variants source, playeri* player) {
	for(auto v : source) {
		if(!v.iskind<uniti>())
			continue;
		auto n = v.counter;
		if(n < 1)
			n = 1;
		for(auto i = 0; i < n; i++)
			placement(bsdata<uniti>::elements + v.value, player);
	}
}

void systemi::limitcapacity() {
	while(true) {
		auto capacity = getsumary(Capacity);
		auto total_ships = getsumary(Ships);
		auto total_ground = getsumary(GroundForces);
		auto total = total_ships + total_ground;
		if(capacity >= total)
			break;
	}
}

static void block_inpassable() {
	for(auto& e : bsdata<systemi>()) {

	}
}

bool systemi::movethrought() const {
	switch(special) {
	case Nebula: case Supernova: return false;
	default: return true;
	}
}

bool systemi::movestop() const {
	switch(special) {
	case AsteroidField: case Supernova: return false;
	default: return true;
	}
}