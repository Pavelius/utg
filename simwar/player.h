#include "costable.h"
#include "variant.h"

#pragma once

struct playeri : nameable, costable {
	int			shield;
	costable	upkeep, upgrade;
	variants	start;
};
extern playeri* player;