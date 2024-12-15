#include "race.h"
#include "move.h"
#include "tag.h"
#include "wear.h"

#pragma once

typedef flagable<2> itemufa;
struct itemi : nameable, raceable, moveable {
	wearn	slot;
	taga	tags;
	racea	need;
	short	coins;
	char	weight, damage, pierce, armor, uses, heal;
};
union item {
	unsigned u;
	struct {
		unsigned char type;
		unsigned char uses : 3;
	};
	constexpr item() : u(0) {}
	constexpr item(unsigned char type) : type(type), uses(geti().uses) {}
	constexpr explicit operator bool() const { return u != 0; }
	void			clear() { u = 0; }
	int				getcost() const;
	constexpr const itemi& geti() const { return bsdata<itemi>::elements[type]; }
	const char*		getname() const { return getnm(geti().id); }
	int				getuses() const;
	constexpr bool	is(tagn v) const { return geti().tags.is(v); }
};
