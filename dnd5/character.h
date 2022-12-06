#include "moveable.h"
#include "statable.h"

#pragma once

struct character : moveable, statable {
	statable	basic;
	char		alignment;
	char		classes[16];
	void		advance(variant object, int level = 0);
	void		clear();
	void		generate();
};
extern character *player;