#include "nameable.h"

#pragma once

enum cost_s : unsigned char {
	Gold, Mana, Happiness, Fame, Trade, Lore, Warfire
};
struct costi : nameable {
	int			width;
};
typedef short costa[Warfire + 1];
struct costable {
	costa cost;
	void operator+=(const costable& v);
	void operator-=(const costable& v);
	bool operator>=(const costable& v) const;
	bool operator<=(const costable& v) const;
	void addcost(cost_s v, int i) { cost[v] += i; }
};