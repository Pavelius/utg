#include "moveable.h"
#include "statable.h"

#pragma once

struct character : moveable, statable {
	statable	basic;
	flagable<16> spells_knows, spells;
	char		alignment;
	char		classes[16];
	void		advance(variant object, int level = 0);
	void		clear();
	void		generate();
};
extern character *player;