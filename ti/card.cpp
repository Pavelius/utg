#include "card.h"
#include "player.h"

bool card::isallow() const {
	if(player)
		return ::player == player;
	return true;
}