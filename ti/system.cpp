#include "main.h"

systemi* systemi::last;

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

void block_move_system() {
	for(auto& e : bsdata<systemi>()) {
		if(e.index == pathfind::Blocked)
			continue;
		if(!e.movethrought())
			pathfind::setmove(e.index, pathfind::Blocked);
	}
}

void block_enemy_system(const playeri* player) {
	for(auto& e : bsdata<systemi>()) {
		if(e.index == pathfind::Blocked)
			continue;
		if(e.player && e.player != player)
			pathfind::setmove(e.index, pathfind::Blocked);
	}
}

static void makewave(pathfind::indext start) {
	pathfind::clearpath();
	block_move_system();
	block_enemy_system(playeri::last);
	pathfind::makewave(start);
}

static void select_you_system_vs_ships(entitya& source) {
	for(auto& e : bsdata<troop>()) {
		if(!bsdata<systemi>::have(e.location))
			continue;
		if(source.have(e.location))
			continue;
		auto ps = (systemi*)e.location;
		if(pathfind::getmove(ps->index) == pathfind::Blocked)
			continue;
		source.add(e.location);
	}
}

static entity* activated_location;

static void choose_units(systemi* location, playeri* player, entitya& source) {
	while(true) {
		char temp[260]; stringbuilder sb(temp);
		source.clear();
		source.select(player, location);
		auto result = source.choose(0, getnm("Cancel"));
		if(!result)
			break;
		if(bsdata<troop>::have(result)) {
			auto p = (troop*)result;
			p->location = activated_location;
			game.updateui();
		}
	}
}

void systemi::moveunits() {
	entitya source;
	activated_location = this;
	while(true) {
		makewave(index);
		source.clear();
		select_you_system_vs_ships(source);
		auto ps = game.choosesystem(source);
		if(!ps)
			break;
		choose_units(ps, playeri::last, source);
	}
	activated_location = 0;
}