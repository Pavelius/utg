#include "bsdata.h"
#include "group.h"

BSDATA(groupi) = {
	{"Individual", 1},
	{"SmallGroup", 5},
	{"MediumGroup", 11},
	{"LargeGroup", 21},
};
assert_enum(groupi, LargeGroup)