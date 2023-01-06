#include "combatable.h"
#include "costable.h"
#include "province.h"
#include "nameable.h"

#pragma once

struct uniti : nameable, combatable, costable {
	costa			upkeep;
};
struct troop : moveable {
	provincei*		moveto;
	const uniti*	type;
};
extern uniti*		lastunit;