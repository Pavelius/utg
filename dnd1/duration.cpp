#include "bsdata.h"
#include "duration.h"

#define TURN 10

BSDATA(durationi) = {
	{"Instant", 0},
	{"Round", 1},
	{"Turn", 1 * TURN},
	{"Turn2", 2 * TURN},
	{"Turn3", 3 * TURN},
	{"Turn12", 3 * TURN},
	{"Turn1d4p1", 2 * TURN, 5 * TURN},
	{"Turn2d6", 2 * TURN, 12 * TURN},
	{"Turn4d4", 4 * TURN, 16 * TURN},
	{"Hour"},
	{"Hour2"},
	{"Hour8"},
	{"Day1"},
	{"Concentration"},
	{"PermanentDuration"},
};
assert_enum(durationi, PermanentDuration)

duration_s duration;