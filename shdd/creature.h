#include "actable.h"
#include "avatarable.h"
#include "class.h"
#include "consumable.h"
#include "race.h"
#include "statable.h"
#include "wearable.h"

#pragma once

struct creature : actable, statable, consumablea, wearable, avatarable {
	class_s		kind;
	race_s		ancestry;
	featable	feats;
	statable	basic;
	short		distance;
	short unsigned enemy;
	static void add(race_s race, class_s kind);
	static void add(const char* id);
	static void add(const struct monsteri* pm);
	void		clear();
	void		damage(int value);
	dice		getdamage(ability_s ability) const;
	creature*	getenemy() const;
	bool		is(feat_s v) const { return feats.is(v); }
	bool		isallow(const item& v) const;
	bool		isenemy(const creature& v) const { return v.is(Enemy) != is(Enemy); }
	bool		isplayer() const { return true; }
	void		kill();
	void		update();
};
extern creature* player;
extern creature* opponent;