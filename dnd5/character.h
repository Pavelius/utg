#include "statable.h"
#include "spell.h"
#include "wearable.h"

#pragma once

struct character : wearable, statable, spellable {
	statable	basic;
	char		alignment;
	char		classes[16];
	void		advance(variant object, int level = 0);
	void		clear();
	void		generate();
};
extern character *player;