#include "deck.h"
#include "statable.h"
#include "tactic.h"
#include "variant.h"

#pragma once

struct playeri : entityi {
	deck			cards;
	int				shield;
	costa			resources, upgrade, income, faith;
	variants		start;
	provincef		visibility;
	int				getindex() const;
};
extern playeri* player;