#include "statable.h"
#include "harmable.h"
#include "nameable.h"
#include "move.h"

#pragma once

struct creature : nameable, harmable, statable {
	harmable suffer;
	void roll(move_s v);
	void move(move_s v);
};
