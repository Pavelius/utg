#include "main.h"

BSDATA(areai) = {
	{"NoArea"},
	{"Slash"},
	{"Circle"},
	{"Ray", {{0,0}, {1, 0}, {2, 0}}},
	{"Splash", {{0, 0}, {1, 0}, {0, 1}}},
	{"Spray"},
};
assert_enum(areai, Spray)