#include "crt.h"
#include "duration.h"

#define TURN 10

BSDATA(durationi) = {
	{"Instant", 0},
	{"Round", 1},
	{"Turn", 1 * TURN},
	{"Turn2", 2 * TURN},
	{"Turn3", 3 * TURN},
	{"Turn1d4p1", 2 * TURN, 5 * TURN},
	{"Turn2d6", 2 * TURN, 12 * TURN},
	{"Turn4d4", 4 * TURN, 16 * TURN},
};
assert_enum(durationi, Turn4d4)

duration_s duration;