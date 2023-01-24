#include "actable.h"
#include "province.h"
#include "tactic.h"
#include "tag.h"

#pragma once

typedef flagable<4>	unitaf;
struct uniti : actable {
	costac			effect, upkeep;
	costa			cost;
	tacticaf		tactics;
	tagf			tags;
	int				fame; // Minimal fame for recruit
	bool			is(tag_s v) const { return tags.is(v); }
	bool			isrecruitable() const { return upkeep[Warfire] != 0; }
};
struct troop : moveable {
	provincei*		moveto;
	const uniti*	type;
	constexpr operator bool() const { return type != 0; }
	const char*		getname() const;
	static const char* getname(const void* p) { return ((troop*)p)->getname(); }
	void			clear() { memset(this, 0, sizeof(*this)); }
	void			paint() const;
};
extern uniti*		lastunit;
troop*				find_troop(const uniti* type, const provincei* province);
troop*				find_troop(const uniti* type, const provincei* province, const playeri* player);