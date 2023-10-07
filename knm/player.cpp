#include "card.h"
#include "player.h"
#include "province.h"
#include "structure.h"

playeri* player;

int	playeri::getmaximum(ability_s v) const {
	switch(v) {
	case Goods:
		return 3 + getbonus(v);
	case Lore:
		return 10;
	case Fame:
		return 10;
	case Resources: case Influence:
		return getbonus(v);
	default:
		return 100;
	}
}