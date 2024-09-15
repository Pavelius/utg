#include "cardaf.h"
#include "deck.h"
#include "statable.h"
#include "province.h"
#include "variant.h"

#pragma once

struct playeri : entityi {
	int				shield;
	char			units;
	costa			resources, income, upgrade, faith;
	variants		start;
	provincef		visibility;
	deck			tactics;
	int				getindex() const;
};
extern playeri* player;

int get_income(costn v);
int get_income_modified(costn v, int result);