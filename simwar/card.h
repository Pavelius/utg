#pragma once

#include "statable.h"
#include "entity.h"
#include "feat.h"

struct playeri;
struct decki;

struct cardi : entityi, featable {
	decki*			deck;
	playeri*		player; // Exclusive for this player
	int				count; // Total count in the deck
	costa			effect;
};