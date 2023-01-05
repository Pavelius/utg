#include "costable.h"

#pragma once

struct playeri : nameable, costable {
	costable	upkeep, upgrade;
};
extern playeri* player;