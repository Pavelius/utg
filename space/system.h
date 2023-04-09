#include "draw_posable.h"
#include "nameable.h"

#pragma once

struct systemi : nameable, posable {
	void		paint() const;
};