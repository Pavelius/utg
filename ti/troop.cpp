#include "main.h"

troop* troop::last;

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
	auto p = addnew();
	p->clear();
	p->id = (const char*)unit;
	p->player = player;
	p->location = location;
	return p;
}

void troop::produce(const uniti* unit) const {
	auto last_system = systemi::last;
	auto last_planet = planeti::last;
	systemi::last = getsystem();
	planeti::last = getplanet();
	unit->placement(unit->abilities[CostCount], false);
	planeti::last = last_planet;
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
	if(player->ishuman())
		draw::warning(format, getname());
	else
		draw::information(format, getname());
	if(destroyed)
		clear();
}