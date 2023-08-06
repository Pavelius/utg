#include "player.h"
#include "main.h"

void playera::filter(const playeri* object, bool keep) {
	auto ps = data;
	for(auto p : *this) {
		if((p == object) != keep)
			continue;
		*ps++ = p;
	}
	count = ps - data;
}

playeri* playera::choose(const char* id) {
	answers an;
	for(auto p : *this)
		an.add(p, p->getname());
	if(player->ishuman())
		return (playeri*)an.choose(getnm(id));
	return (playeri*)an.random();
}