#include "card.h"
#include "player.h"
#include "province.h"
#include "structure.h"

playeri* player;
playeri* speaker;

int	playeri::getmaximum(abilityn v) const {
	switch(v) {
	case Goods:
		return 3 + getbonus(v);
	case Fame:
		return 10;
	case Resources: case Influence:
		return getbonus(v);
	case Tactic:
		return 6;
	default:
		return 100;
	}
}

int	playeri::getarmy(abilityn v) const {
	return current.abilities[v] + getupgrade(v);
}