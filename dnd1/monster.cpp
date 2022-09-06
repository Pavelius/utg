#include "main.h"

void update_monsters_stats() {
	for(auto& e : bsdata<monsteri>()) {
		e.abilities[AC] = 9 - e.abilities[AC];
	}
}