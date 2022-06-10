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
		troop::create(unit, player, this);
	else {
		auto planet = getbestplanet();
		if(planet)
			troop::create(unit, player, planet);
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

int systemi::getcapacity(bool include_docks) const {
	entitya source;
	source.clear();
	source.select(player, this);
	auto capacity = source.getsummary(Capacity);
	if(include_docks) {
		entitya ground;
		ground.select(player, this, Structures);
		capacity += ground.getsummary(CapacityShips);
	}
	return capacity;
}

void systemi::limitcapacity() {
	entitya source;
	while(true) {
		// Get all ships and ground forces in space and calculate summary capacity
		source.clear();
		source.select(player, this);
		if(!source)
			break;
		auto capacity = source.getsummary(Capacity);
		// Get capacity of docks for fighters
		entitya ground;
		ground.select(player, this, Structures);
		auto capacity_ships = ground.getsummary(CapacityShips);
		// Get only loaded units and calculate total
		source.matchload(true);
		auto total_ships = source.getsummary(Ships);
		if(capacity_ships > 0) {
			if(capacity_ships > total_ships)
				capacity_ships = total_ships;
			total_ships -= capacity_ships;
		}
		auto total_ground = source.getsummary(GroundForces);
		auto total = total_ships + total_ground;
		if(capacity >= total)
			break;
		if(!source)
			break;
		// Remove one unit
		source[0]->clear();
	}
}

bool systemi::movethrought() const {
	switch(special) {
	case Nebula: case Supernova: case AsteroidField: return false;
	default: return true;
	}
}

bool systemi::movestop() const {
	switch(special) {
	case AsteroidField: case Supernova: return false;
	default: return true;
	}
}

void systemi::blockmove() {
	for(auto& e : bsdata<systemi>()) {
		if(!e)
			continue;
		if(!e.movethrought())
			pathfind::setmove(e.index, pathfind::Blocked);
	}
}

void systemi::blockenemy(const playeri* player) {
	for(auto& e : bsdata<systemi>()) {
		if(!e)
			continue;
		if(e.player && e.player != player)
			pathfind::setmove(e.index, pathfind::Blocked);
	}
}

void systemi::markzerocost(const playeri* player) {
	for(auto& e : bsdata<systemi>()) {
		if(!e)
			continue;
		if(e.player == player) {
			pathfind::setmove(e.index, 0);
			pathfind::addwave(e.index);
		}
	}
}

systemi* systemi::findbyindex(pathfind::indext i) {
	for(auto& e : bsdata<systemi>()) {
		if(e.index == i)
			return &e;
	}
	return 0;
}