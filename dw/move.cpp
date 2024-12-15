#include "bsdata.h"
#include "move.h"

BSDATA(movetypei) = {
	{"BasicMove"},
	{"StartMove"},
	{"AdvancedMove"},
};
assert_enum(movetypei, AdvancedMove)