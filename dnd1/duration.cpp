#include "bsdata.h"
#include "duration.h"
#include "formula.h"
#include "script.h"

#define TURN 10
#define HOUR 6*TURN

BSDATA(durationi) = {
	{"Instant", 0},
	{"Round", 1},
	{"Turn1", 1 * TURN},
	{"Turn2", 2 * TURN},
	{"Turn3", 3 * TURN},
	{"Turn6", 6 * TURN},
	{"Turn9", 9 * TURN},
	{"Turn12", 3 * TURN},
	{"Turn1d4p1", 2 * TURN, 5 * TURN},
	{"Turn2d6", 2 * TURN, 12 * TURN},
	{"Turn4d4", 4 * TURN, 16 * TURN},
	{"Hour", 1 * HOUR},
	{"Hour2", 2 * HOUR},
	{"Hour8", 8 * HOUR},
	{"Day1"},
	{"Roll1d3", 1, 3},
	{"Roll1d4", 1, 4},
	{"Roll1d6", 1, 6},
	{"Roll1d8", 1, 8},
	{"Roll1d10", 1, 10},
	{"Roll1d12", 1, 12},
	{"Roll1d20", 1, 20},
	{"Concentration"},
	{"PermanentDuration"},
};
assert_enum(durationi, PermanentDuration)

durationn duration;

template<> void fnscript<durationi>(int index, int bonus) {
	last_number = bsdata<durationi>::elements[index].roll() + bonus;
}