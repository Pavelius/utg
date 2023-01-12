#include "nameable.h"

#pragma once

enum statf : unsigned char {
	PerTurn, Resource,
};
enum cost_s : unsigned char {
	Gold, Mana, Happiness, Fame, Faith, Lore, Warfire,
	Strenght, Damage, Health, Charge, Archery, Level, Armor,
	Movement, Sail, Explore, ExploreNext, Siege,
	Wood, Iron, Silver, Mithril, Horses, Mandrake,
	Build, Size,
	Limit,
};
struct costi : nameable {
	unsigned	flags;
	int			width;
	const char*	format;
	int			frame = -1;
	bool		is(statf v) const { return (flags & (1 << v)) != 0; }
};
typedef short costa[Limit + 1];
typedef char costac[Limit + 1];
void addvalue(costa& v1, const costa& v2);
void addvalue(costa& v1, const costac& v2);
void addvalue(costac& v1, const costac& v2);
bool isenought(const costa& v1, const costa& v2);
void subvalue(costa& v1, const costa& v2);
void subvalue(costac& v1, const costac& v2);