#include "main.h"

troop* last_troop;

static troop* addnew() {
	for(auto& e : bsdata<troop>()) {
		if(!e)
			return &e;
	}
	return bsdata<troop>::add();
}

troop* troop::create(const char* id, playeri* player) {
	auto pu = bsdata<uniti>::find(id);
	if(!pu)
		return 0;
	return create(pu, player, 0);
}

troop* troop::create(const uniti* unit, playeri* player, entity* location) {
	auto index = getbsi(unit);
	if(index <= 9)
		unit = player->getunit(index);
	auto p = addnew();
	p->clear();
	p->id = (const char*)unit;
	p->player = player;
	p->location = location;
	return p;
}

void troop::produce(const uniti* unit) const {
	auto last_system = systemi::last;
	auto push_planet = last_planet;
	systemi::last = getsystem();
	last_planet = getplanet();
	unit->placement(unit->abilities[CostCount], false);
	last_planet = push_planet;
	systemi::last = last_system;
}

void troop::hit() {
	const char* format = "%1 %-Destoyed";
	auto destroyed = false;
	if(is(SustainDamage) && !is(Exhaust)) {
		set(Exhaust);
		format = "%1 %-Damaged";
	} else
		destroyed = true;
	event(-1, format);
	if(destroyed)
		clear();
}