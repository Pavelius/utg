#include "planet.h"
#include "player.h"
#include "system.h"
#include "math.h"
#include "troop.h"
#include "unit.h"
#include "unit_type.h"

uniti* last_unit;

int	uniti::getweight() const { return abilities[Cost] * 2 / imax(1, (int)abilities[CostCount]); }

void uniti::placement(int count, bool need_update_ui) const {
	if(!count)
		count = 1;
	switch(type) {
	case GroundForces: case Structures:
		last_planet->focusing();
		for(auto i = 0; i < count; i++)
			create_troop(this, player, last_planet);
		if(need_update_ui)
			update_ui();
		break;
	case Ships:
		last_system->focusing();
		for(auto i = 0; i < count; i++)
			create_troop(this, player, last_system);
		if(need_update_ui)
			update_ui();
		break;
	}
}

int uniti::getscore(abilityn v) const {
	abilityn v1 = (abilityn)(v + 1);
	auto count = abilities[v1];
	if(!count)
		count = 1;
	return (2 + 10 - abilities[v]) * count;
}

int uniti::getscore(tag_s v, int n) const {
	if(tags.is(v))
		return n;
	return 0;
}

int uniti::getspacepower() const {
	auto r = getscore(Combat);
	r += getscore(SustainDamage, 2);
	return r;
}

prototype* uniti::getprototype() const {
	if(bsdata<prototype>::have(this))
		return bsdata<prototype>::elements + bsdata<prototype>::source.indexof(this);
	return 0;
}

int	uniti::getindex() const {
	auto p = getprototype();
	if(p)
		return (this - p->units);
	return -1;
}