#include "drawable.h"
#include "nameable.h"

#pragma once

struct systemi : nameable, drawable {
	void		paint() const;
};