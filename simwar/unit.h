#include "province.h"
#include "tactic.h"
#include "tag.h"

#pragma once

typedef flagable<4>	unitaf;
struct uniti : nameable {
	costac			effect, cost, upkeep;
	tacticaf		tactics;
	tagf			tags;
};
struct troop : moveable {
	provincei*		moveto;
	const uniti*	type;
	constexpr operator bool() const { return type != 0; }
	void			clear() { memset(this, 0, sizeof(*this)); }
	void			paint() const;
};
extern uniti*		lastunit;
troop*				find_troop(const uniti* type, const provincei* province);
troop*				find_troop(const uniti* type, const provincei* province, const playeri* player);