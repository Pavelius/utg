#include "nameable.h"

#pragma once

enum cost_s : unsigned char {
	Gold, Mana, Happiness, Fame, Trade, Lore, Warfire,
	Strenght, Damage, Health, Charge, Archery, Level, Armor,
	Movement, Explore, Siege, Size,
	Limit,
};
struct costi : nameable {
	int			width;
	const char*	format;
};
typedef short costa[Limit + 1];
struct costable {
	costa effect;
	void operator+=(const costa& v);
	void operator+=(const costable& v) { operator+=(v.effect); }
	void operator-=(const costa& v);
	void operator-=(const costable& v) { operator-=(v.effect); }
	bool operator>=(const costable& v) const;
	bool operator<=(const costable& v) const;
	void addcost(cost_s v, int i) { effect[v] += i; }
};