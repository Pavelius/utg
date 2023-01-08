#include "statable.h"
#include "variant.h"

#pragma once

struct playeri : nameable {
	int			shield;
	costa		resources, income, upgrade, trade;
	variants	start;
};
extern playeri* player;