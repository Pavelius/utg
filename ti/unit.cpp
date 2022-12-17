#include "main.h"

uniti* uniti::last;

void uniti::placement(int count, bool updateui) const {
	if(!count)
		count = 1;
	switch(type) {
	case GroundForces: case Structures:
		game.focusing(planeti::last);
		for(auto i = 0; i < count; i++)
			troop::create(this, player, planeti::last);
		if(updateui)
			game.updateui();
		draw::information(getnm("PlaceUnits"), getnm(player->id), getnm(id), count);
		break;
	case Ships:
		game.focusing(systemi::last);
		for(auto i = 0; i < count; i++)
			troop::create(this, player, systemi::last);
		if(updateui)
			game.updateui();
		draw::information(getnm("PlaceUnits"), getnm(player->id), getnm(id), count);
		break;
	}
}