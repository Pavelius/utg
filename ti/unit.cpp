#include "crt.h"
#include "main.h"
#include "unit.h"

int	uniti::getweight() const { return abilities[Cost] * 2 / imax(1, (int)abilities[CostCount]); }

void uniti::placement(int count, bool updateui) const {
	if(!count)
		count = 1;
	switch(type) {
	case GroundForces: case Structures:
		game.focusing(last_planet);
		for(auto i = 0; i < count; i++)
			troop::create(this, player, last_planet);
		if(updateui)
			game.updateui();
		break;
	case Ships:
		game.focusing(last_system);
		for(auto i = 0; i < count; i++)
			troop::create(this, player, last_system);
		if(updateui)
			game.updateui();
		break;
	}
}

int uniti::getscore(ability_s v) const {
	ability_s v1 = (ability_s)(v + 1);
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