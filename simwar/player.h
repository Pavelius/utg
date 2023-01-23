#include "statable.h"
#include "tactic.h"
#include "variant.h"

#pragma once

struct playeri : nameable {
	int			shield;
	costa		resources, upgrade;
	costac		income, faith;
	variants	start;
	tacticaf	tactics;
	provincef	visibility;
	int			getindex() const;
};
extern playeri* player;