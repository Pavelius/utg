#include "attackable.h"
#include "feat.h"
#include "group.h"
#include "statable.h"
#include "variant.h"

#pragma once

typedef short		monstersa[256];
struct monsteri : nameable {
	interval		dungeon, wilderness;
	variants		feats, dungeon_group, dungeon_lair, wilderness_group, wilderness_lair;
	racei*			origin;
	const char*		nature;
	const monsteri*	look;
	char			treasure[8];
	char			magic;
	bool			is(feat_s v) const;
	bool			isfalselook() { return look != getlook(); }
	interval		getcount(group_s v) const;
	const monsteri*	getlook() const;
	const char*		getnature() const;
};
extern monsteri* encountered_monster;
extern monstersa monster_encountered, monster_killed;