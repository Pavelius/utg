#include "attackable.h"
#include "feat.h"
#include "group.h"
#include "statable.h"
#include "variant.h"

#pragma once

struct monsteri : nameable {
	interval	dungeon, wilderness;
	variants	feats, dungeon_group, dungeon_lair, wilderness_group, wilderness_lair;
	racei*		origin;
	char		treasure[8];
	char		magic;
	interval	getcount(group_s v) const;
};
extern monsteri* encountered_monster;