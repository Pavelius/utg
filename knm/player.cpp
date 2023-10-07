#include "card.h"
#include "player.h"
#include "province.h"
#include "structure.h"

playeri* player;

int	playeri::getmaximum(ability_s v) const {
	switch(v) {
	case Goods:
		return 3 + getsummary(v);
	case Lore:
		return 10;
	case Fame:
		return 10;
	case Resources: case Influence:
		return getsummary(v);
	default:
		return 100;
	}
}

int	playeri::getsummary(ability_s v) const {
	auto result = 0;
	for(auto& e : bsdata<provincei>()) {
		if(e.player != this || e.index == 0xFFFF)
			continue;
		result += e.get(v);
	}
	for(auto& e : bsdata<structure>()) {
		auto p = e.getprovince();
		if(!p || p->player != this)
			continue;
		result += e.get(v);
	}
	for(auto& e : bsdata<cardi>()) {
		if(!e.location || e.player != this)
			continue;
		result += e.get(v);
	}
	return result;
}