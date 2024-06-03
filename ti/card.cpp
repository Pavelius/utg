#include "card.h"
#include "player.h"

bool componenti::isallow() const {
	if(player)
		return ::player == player;
	return true;
}