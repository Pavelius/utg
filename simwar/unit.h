#include "province.h"
#include "statable.h"
#include "nameable.h"

#pragma once

struct uniti : nameable {
	costac			effect, cost, upkeep;
};
struct troop : moveable {
	provincei*		moveto;
	const uniti*	type;
	void			paint() const;
};
extern uniti*		lastunit;