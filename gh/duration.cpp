#include "main.h"

BSDATA(durationi) = {
	{"Instant"},
	{"Infinite"},
	{"Round"},
	{"Use"},
};
assert_enum(durationi, Use)