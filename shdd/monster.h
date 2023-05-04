#include "class.h"
#include "dice.h"
#include "race.h"
#include "statable.h"
#include "variant.h"

#pragma once

struct monsteri : nameable, statable {
	class_s		kind;
	race_s		ancestry;
	variants	feats;
};
