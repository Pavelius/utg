#pragma once
#include "moveable.h"
#include "statable.h"

struct character : moveable, statable {
	statable	basic;
	char		classes[16];
	void		clear();
	void		generate();
};
extern character *player;