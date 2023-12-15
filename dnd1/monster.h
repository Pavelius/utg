#include "dice.h"
#include "feat.h"
#include "statable.h"
#include "variant.h"

#pragma once

struct monsteri : nameable {
	dice		dungeon, wilderness;
	variants	feats;
};
