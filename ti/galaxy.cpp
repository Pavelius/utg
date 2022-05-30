#include "main.h"

using namespace pathfind;

void galaxy::cleargalaxy() {
	for(auto i = 0; i < pathfind::maxcount; i++)
		tiles[i] = Blocked;
}

systemi* galaxy::get(indext i) const {
	if(i == Blocked)
		return 0;
	return bsdata<systemi>::elements + tiles[i];
}

void galaxy::set(indext i, const systemi* system) {
	if(i == Blocked)
		return;
	if(!system)
		tiles[i] = Blocked;
	else
		tiles[i] = bsdata<systemi>::elements - system;
}