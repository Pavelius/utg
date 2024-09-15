#include "cardaf.h"
#include "deck.h"
#include "statable.h"
#include "province.h"
#include "variant.h"

#pragma once

const int player_maximum = 16;

struct playeri : entityi {
	int			shield, units;
	costa		resources, income, faith;
	variants	start;
	provincef	visibility;
	deck		tactics;
	int			getindex() const;
};
extern playeri* player;

int get_income(costn v);
int get_income_modified(costn v, int result);