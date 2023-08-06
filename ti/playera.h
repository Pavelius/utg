#include "crt.h"

#pragma once

struct playeri;

struct playera : adat<playeri*, 6> {
	playeri*	choose(const char* title);
	void		filter(const playeri* object, bool keep);
	void		shuffle() { zshuffle(data, count); }
};
extern playera	players, origin_players;