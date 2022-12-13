#include "statable.h"
#include "actable.h"
#include "rangeable.h"

#pragma once

struct vagabond : actable, statable, rangeable {
	void clear() { memset(this, 0, sizeof(*this)); }
	void roll(move_s v);
	void move(move_s v);
};
extern vagabond* player;
