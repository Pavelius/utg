#include "player.h"
#include "province.h"

playeri* player;

int	playeri::getmaximum(ability_s v) const {
	switch(v) {
	case Trade: return 3;
	default: return 100;
	}
}

int	playeri::getprovincesummary(ability_s v) const {
	auto result = 0;
	for(auto& e : bsdata<provincei>()) {
		if(e.player != this)
			continue;
		result += e.get(v);
	}
	return result;
}