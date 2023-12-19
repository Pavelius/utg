#include "attackable.h"
#include "feat.h"
#include "statable.h"
#include "variant.h"

#pragma once

struct monsteri : nameable, attackable {
	interval	dungeon, wilderness;
	variants	feats;
	racei*		origin;
	monsteri*	basic;
	char		treasure[8];
};
extern monsteri* encountered_monster;