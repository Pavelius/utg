#include "statable.h"
#include "variant.h"

#pragma once

struct playeri : nameable {
	int			shield;
	costa		resources, upgrade;
	costac		income, trade, artifacts;
	variants	start;
};
extern playeri* player;