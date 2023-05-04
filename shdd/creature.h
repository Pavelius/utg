#include "class.h"
#include "race.h"
#include "statable.h"
#include "wearable.h"

#pragma once

struct creature : statable, wearable {
	class_s		kind;
	race_s		ancestry;
	featable	feats;
	statable	basic;
	static void add(race_s race, class_s kind);
	static void add(const char* id);
	static void add(const struct monsteri* pm);
	void		clear();
	bool		is(feat_s v) const { return feats.is(v); }
	bool		isallow(const item& v) const;
	bool		isplayer() const { return true; }
	void		update();
};
extern creature* player;