#include "province.h"
#include "tactic.h"

#pragma once

struct uniti : nameable {
	costac			effect, cost, upkeep;
	tacticaf		tactics;
};
struct troop : moveable {
	provincei*		moveto;
	const uniti*	type;
	constexpr operator bool() const { return type != 0; }
	void			paint() const;
};
extern uniti*		lastunit;
typedef flagable<4>	unitaf;