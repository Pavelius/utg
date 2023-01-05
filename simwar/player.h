#include "province.h"

#pragma once

struct playeri : nameable, costable {
	costable	upkeep;
};
extern playeri* player;