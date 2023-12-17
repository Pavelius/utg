#include "attackable.h"
#include "dice.h"
#include "feat.h"
#include "statable.h"
#include "variant.h"

#pragma once

struct monsteri : nameable, attackable {
	dice		dungeon, wilderness;
	variants	feats;
	racei*		origin;
	monsteri*	basic;
	char		treasure[8];
};
extern monsteri* encountered_monster;