#include "crt.h"

#pragma once

enum stat_s : unsigned char {
	Gold, Food, Wood, Mana, Happiness, Fame, Trade, Lore,
	Stone, Iron, Gems,
};

struct nameable {
	const char*			id;
};
struct stati : nameable {
	int					format;
};
struct statable {
	short				data[Gems + 1];
	operator bool() const;
	void operator+=(const statable& v);
	void operator-=(const statable& v);
	bool operator>=(const statable& v) const;
	bool operator<=(const statable& v) const;
	void				add(stat_s v, int i) { data[v] += i; }
	int					get(stat_s v) const { return data[v]; }
};
struct building : nameable {
	statable			effect, upkeep, cost;
};
struct player : nameable, statable {
	statable			income;
	static player*		last;
};