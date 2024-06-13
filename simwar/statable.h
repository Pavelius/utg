#include "nameable.h"

#pragma once

class stringbuilder;

enum statf : unsigned char {
	PerTurn, Resource,
};
enum cost_s : unsigned char {
	Gold, Mana, Happiness, Fame, Lore, Warfire,
	Strenght, Sword, Shield, Death, Experience,
	Movement, Sail, Explore, ExploreNext,
	Recruit, Size,
	Limit,
};
struct costi : nameable {
	unsigned	flags;
	int			width;
	const char*	format;
	int			frame = -1;
	bool		is(statf v) const { return (flags & (1 << v)) != 0; }
	void		getinfo(stringbuilder& sb) const;
};
typedef short costa[Limit + 1];
void addvalue(costa& v1, const costa& v2);
bool isenought(const costa& v1, const costa& v2);
bool isempthy(const costa& v);
void subvalue(costa& v1, const costa& v2);