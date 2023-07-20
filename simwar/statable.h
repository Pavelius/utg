#include "nameable.h"

#pragma once

class stringbuilder;

enum statf : unsigned char {
	PerTurn, Resource,
};
enum cost_s : unsigned char {
	Gold, Mana, Happiness, Fame, Lore, Warfire,
	Strenght, Army, Sword, Shield, Experience,
	Movement, Sail, Explore, ExploreNext, Siege,
	Wood, Iron, Silver, Mithril, Horses, Mandrake,
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
typedef char costac[Limit + 1];
void addvalue(costa& v1, const costa& v2);
void addvalue(costa& v1, const costac& v2);
void addvalue(costac& v1, const costac& v2);
bool isenought(const costa& v1, const costa& v2);
bool isenought(const costa& v1, const costac& v2);
bool isempthy(const costa& v);
bool isempthy(const costac& v);
void subvalue(costa& v1, const costa& v2);
void subvalue(costac& v1, const costac& v2);
void subvalue(costa& v1, const costac& v2);