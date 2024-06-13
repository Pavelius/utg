#pragma once

#include "statable.h"
#include "entity.h"
#include "tag.h"

struct playeri;
struct decki;

struct cardi : entityi {
	decki*			deck;
	playeri*		player; // Exclusive for this player
	int				count; // Total count in the deck
	costa			effect;
	tagf			tags;
};