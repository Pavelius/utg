#include "statable.h"
#include "tactic.h"
#include "variant.h"

#pragma once

struct playeri : nameable {
	int			shield;
	costa		resources, upgrade;
	costac		income, faith, artifacts;
	variants	start;
	tacticaf	tactics;
};
extern playeri* player;