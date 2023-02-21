#include "nameable.h"

#pragma once

enum duration_s : unsigned char {
	Instant,
	Round,
	Turn, Turn2, Turn3, Turn1d4p1, Turn2d6, Turn4d4,
	Hour, Hour2, Hour8,
	Concentration, PermanentDuration,
};
struct durationi : nameable {
	short	from, to;
};
extern duration_s duration;
