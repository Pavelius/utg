#include "main.h"

int entity::get(ability_s v) const {
	if(bsdata<troop>::have(this))
		return ((troop*)this)->type->abilities[v];
	return 0;
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

void troop::create(const char* id) {
	clear();
	type = bsdata<uniti>::find(id);
}