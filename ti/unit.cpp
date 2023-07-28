#include "main.h"

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
		//draw::information(getnm("PlaceUnits"), getnm(player->id), getnm(id), count);
		break;
	case Ships:
		game.focusing(systemi::last);
		for(auto i = 0; i < count; i++)
			troop::create(this, player, systemi::last);
		if(updateui)
			game.updateui();
		//draw::information(getnm("PlaceUnits"), getnm(player->id), getnm(id), count);
		break;
	}
}

int uniti::getscore(ability_s v) const {
	ability_s v1 = (ability_s)(v + 1);
	auto count = abilities[v1];
	if(!count)
		count = 1;
	return (10 - abilities[v]) * count;
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