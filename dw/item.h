#include "race.h"
#include "move.h"
#include "wear.h"

#pragma once

enum itemuf_s : unsigned char {
	HooksAndSpikes, Sharp, PerfectlyWeigthed, SerratedEdges, Glows, Huge, Versatile, WellCrafted,
};
typedef flagable<1> itemufa;
struct itemi : raceable, moveable {
	wear_s	slot;
	taga	tags;
	racea	need;
	short	coins;
	char	weight, damage, pierce, armor, uses, heal;
};
union item {
	unsigned u;
	struct {
		unsigned char type;
		unsigned char signature : 1;
		unsigned char used : 3;
		itemufa feats;
	};
	constexpr item() : u(0) {}
	constexpr item(unsigned char type) : type(type), signature(0), used(0), feats() {}
	constexpr explicit operator bool() const { return u != 0; }
	void clear() { u = 0; }
	constexpr const itemi& geti() const { return bsdata<itemi>::elements[type]; }
	constexpr bool is(itemuf_s v) const { return feats.is(v); }
	constexpr bool is(tag_s v) const { return geti().tags.is(v); }
};
