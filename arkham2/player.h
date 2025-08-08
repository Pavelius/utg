#pragma once

#include "statable.h"

struct playeri : statable {
	char type;
	char focus[3];
};

extern playeri player;

void make_roll(abilityn v, int bonus);
