#include "interval.h"
#include "nameable.h"

#pragma once

enum duration_s : unsigned char {
	Instant,
	Round,
	Turn, Turn2, Turn3, Turn12, Turn1d4p1, Turn2d6, Turn4d4,
	Hour, Hour2, Hour8, Day1,
	Concentration, PermanentDuration,
};
struct durationi : nameable, interval {
};
extern duration_s duration;
