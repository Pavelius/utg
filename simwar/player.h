#include "costable.h"
#include "variant.h"

#pragma once

struct playeri : nameable, costable {
	int			shield;
	costable	upkeep, upgrade, trade;
	variants	start;
};
extern playeri* player;