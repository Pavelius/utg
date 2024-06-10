#pragma once

#include "nameable.h"

struct playeri;

struct cardi : nameable {
	playeri*		player; // Exclusive for this player
	int				count; // Total count in the deck
};