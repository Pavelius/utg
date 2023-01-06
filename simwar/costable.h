#include "nameable.h"

#pragma once

enum cost_s : unsigned char {
	Gold, Mana, Happiness, Fame, Trade, Lore, Warfire
};
struct costi : nameable {
	int				width;
	const char*		format;
};
typedef short costa[Warfire + 1];
struct costable {
	costa cost;
	void operator+=(const costa& v);
	void operator+=(const costable& v) { operator+=(v.cost); }
	void operator-=(const costa& v);
	void operator-=(const costable& v) { operator-=(v.cost); }
	bool operator>=(const costable& v) const;
	bool operator<=(const costable& v) const;
	void addcost(cost_s v, int i) { cost[v] += i; }
};