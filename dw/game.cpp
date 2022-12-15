#include "game.h"
#include "main.h"

int gamei::getaverage(ability_s v) const {
	auto count = 0, total = 0;
	for(auto& e : bsdata<creature>()) {
		if(!e)
			continue;
		total += e.get(v);
		count += 1;
	}
	if(!count)
		return 0;
	return total / count;
}