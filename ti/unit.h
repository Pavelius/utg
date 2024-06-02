#include "ability.h"
#include "tag.h"

#pragma once

struct playeri;
struct prototype;

enum unit_type_s : unsigned char;

struct uniti : nameable {
	char			abilities[Capacity + 1];
	unit_type_s		type;
	taga			tags;
	playeri*		race;
	const uniti*	replace;
	int				getcost() const { return abilities[Cost]; }
	int				getindex() const;
	prototype*		getprototype() const;
	int				getspacepower() const;
	int				getweight() const;
	void			placement(int count, bool updateui = true) const;
	bool			stackable() const { return abilities[CostCount] > 1; }
private:
	int				getscore(ability_s v) const;
	int				getscore(tag_s v, int n) const;
};
struct prototype {
	uniti			units[10];
};
extern uniti* last_unit;