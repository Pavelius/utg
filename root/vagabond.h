#include "statable.h"
#include "harmable.h"
#include "move.h"
#include "person.h"
#include "rangeable.h"

#pragma once

struct vagabond : person, statable, rangeable {
	void roll(move_s v);
	void move(move_s v);
};
