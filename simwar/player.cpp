#include "deck.h"
#include "game.h"
#include "relation.h"

playeri* player;
deck neutral_tactics;

int	playeri::getindex() const {
	return this - bsdata<playeri>::elements;
}

int	playeri::getrelation(const playeri* player) const {
	auto i = relation_index(this, player);
	return (i == -1) ? 0 : current_relation[i];
}