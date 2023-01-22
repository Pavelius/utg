#include "game.h"

playeri* player;

int	playeri::getindex() const {
	return this - bsdata<playeri>::elements;
}