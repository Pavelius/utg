#include "deck.h"
#include "statable.h"
#include "province.h"
#include "tactic.h"
#include "variant.h"

#pragma once

struct playeri : entityi {
	deck			cards;
	int				shield;
	char			units;
	costa			resources, income, upgrade, faith;
	variants		start;
	provincef		visibility;
	int				getindex() const;
};
extern playeri* player;

int get_income(costn v);
int get_income_modified(costn v, int result);