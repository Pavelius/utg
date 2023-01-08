#include "province.h"
#include "statable.h"
#include "nameable.h"

#pragma once

struct uniti : nameable {
	costa			effect, upkeep;
};
struct troop : moveable {
	provincei*		moveto;
	const uniti*	type;
	void			paint() const;
};
extern uniti*		lastunit;