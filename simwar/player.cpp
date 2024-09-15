#include "deck.h"
#include "game.h"

playeri* player;
deck neutral_tactics;

int	playeri::getindex() const {
	return this - bsdata<playeri>::elements;
}