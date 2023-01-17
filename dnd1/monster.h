#include "dice.h"
#include "feat.h"
#include "statable.h"

#pragma once

struct monsteri : nameable, statable {
	featable	feats;
	dice		dungeon, wilderness;
};
