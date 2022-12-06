#include "actable.h"
#include "point.h"

#pragma once

struct moveable : point, actable {
	void	setposition(point v) { x = v.x; y = v.y; }
};
