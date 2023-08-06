#include "pushvalue.h"
#include "planet.h"
#include "player.h"
#include "system.h"
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
	pushvalue push_system(last_system, getsystem());
	pushvalue push_planet(last_planet, getplanet());
	unit->placement(unit->abilities[CostCount], false);
}

void troop::hit() {
	const char* format = "%1 %-Destoyed";
	auto destroyed = false;
	if(is(SustainDamage) && !is(Exhaust)) {
		set(Exhaust, true);
		if(!is(IgnoreDirectHit) && reaction("DirectHit", 0, player, getname()))
			destroyed = true;
		else
			format = "%1 %-Damaged";
	} else
		destroyed = true;
	status(-1, format);
	if(destroyed)
		clear();
}