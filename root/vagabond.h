#include "statable.h"
#include "actable.h"
#include "rangeable.h"

#pragma once

struct vagabond : actable, statable, rangeable {
	movea moves;
	void clear() { memset(this, 0, sizeof(*this)); }
	bool isallow(move_s v) const;
	void roll(move_s v);
	void move(move_s v);
};
extern vagabond* player;
