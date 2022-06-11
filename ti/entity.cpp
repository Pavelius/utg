#include "main.h"

int entity::get(ability_s v) const {
	if(bsdata<troop>::have(this))
		return ((uniti*)id)->abilities[v];
	else if(bsdata<uniti>::have(this))
		return ((uniti*)this)->abilities[v];
	return 0;
}

int	entity::getsumary(unit_type_s v) const {
	auto result = 0;
	for(auto& e : bsdata<troop>()) {
		if(e.location != this)
			continue;
		if(e.getunit()->type==v)
			result++;
	}
	return result;
}

int	entity::getsumary(ability_s v) const {
	auto result = 0;
	for(auto& e : bsdata<troop>()) {
		if(e.location != this)
			continue;
		result += e.get(v);
	}
	return result;
}

systemi* entity::getsystem() const {
	if(!this)
		return 0;
	if(bsdata<troop>::have(this))
		return ((troop*)this)->location->getsystem();
	else if(bsdata<planeti>::have(this))
		return ((planeti*)this)->location->getsystem();
	else if(bsdata<systemi>::have(this))
		return (systemi*)this;
	return 0;
}

const char* entity::getid() const {
	if(id)
		return id;
	return "";
}

planeti* entity::getplanet() const {
	if(!this)
		return 0;
	if(bsdata<troop>::have(this))
		return ((troop*)this)->location->getplanet();
	else if(bsdata<planeti>::have(this))
		return (planeti*)this;
	return 0;
}

planet_trait_s entity::gettrait() const {
	if(bsdata<planeti>::have(this))
		return ((planeti*)this)->trait;
	return NoTrait;
}

bool entity::is(flag_s v) const {
	if(bsdata<planeti>::have(this))
		return ((planeti*)this)->flags.is(v);
	return false;
}

color_s entity::getspeciality() const {
	if(bsdata<planeti>::have(this))
		return ((planeti*)this)->speciality;
	return NoTech;
}

int	entity::get(indicator_s v) const {
	if(bsdata<planeti>::have(this))
		return ((planeti*)this)->get(v);
	return 0;
}

int entity::fight(int chance, int count, int reroll) {
	if(!chance)
		return 0;
	if(!count)
		count = 1;
	auto n = 0;
	for(auto i = 0; i < count; i++) {
		auto r = reroll + 1;
		while(r-- > 0) {
			if((1 + (rand() % 10)) >= chance) {
				n++;
				break;
			}
		}
	}
	return n;
}

troop* entity::sibling(troop* pb) const {
	if(!pb)
		pb = bsdata<troop>::begin();
	else
		pb = pb + 1;
	auto pe = bsdata<troop>::end();
	while(pb < pe) {
		if(pb->location == this)
			return pb;
		pb++;
	}
	return 0;
}

const char* entity::getname() const {
	if(bsdata<uniti>::have(id))
		return getnm(((uniti*)id)->id);
	if(bsdata<actioncard>::have(id))
		return getnm(((actioncard*)id)->id);
	return getnm(getid());
}

const uniti* entity::getunit() const {
	if(bsdata<uniti>::have(id))
		return (uniti*)id;
	return 0;
}

const actioncard* entity::getactioncard() const {
	if(bsdata<actioncard>::have(id))
		return (actioncard*)id;
	return 0;
}

int	entity::getproduction() const {
	auto result = 0;
	auto pu = getunit();
	if(pu) {
		result += pu->abilities[Production];
		if(pu->tags.is(AddPlanetResourceToProduction)) {
			auto planet = getplanet();
			if(planet)
				result += planet->get(Resources);
		}
	}
	return result;
}

void entity::add(answers& an) {
	auto planet = getplanet();
	if(planet)
		an.add(this, "%1 (%2)", getname(), planet->getname());
	else
		an.add(this, getname());
}

playeri* entity::getenemy() const {
	for(auto& e : bsdata<troop>()) {
		if(e.location == this && e.player != player)
			return e.player;
	}
	return 0;
}