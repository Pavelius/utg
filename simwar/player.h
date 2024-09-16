#include "cardaf.h"
#include "deck.h"
#include "statable.h"
#include "province.h"
#include "variant.h"

#pragma once

struct playeri : entityi {
	int			shield, units;
	char		explore, provinces;
	costa		resources, income, faith;
	variants	start;
	provincef	visibility;
	deck		tactics;
	int			getindex() const;
	int			getrelation(const playeri* player) const;
};
extern playeri* player;

int get_income(costn v);
int get_income_modified(costn v, int result);