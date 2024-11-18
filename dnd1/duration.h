#include "interval.h"
#include "nameable.h"

#pragma once

enum durationn : unsigned char {
	Instant,
	Round,
	Turn, Turn2, Turn3, Turn6, Turn9, Turn12, Turn1d4p1, Turn2d6, Turn4d4,
	Hour, Hour2, Hour8, Day1,
	Roll1d3, Roll1d4, Roll1d6, Roll1d8, Roll1d10, Roll1d12, Roll1d20,
	Roll2d6,
	Concentration, PermanentDuration,
};
struct durationi : nameable, interval {
};
extern durationn duration;
