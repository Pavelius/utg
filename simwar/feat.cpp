#include "bsdata.h"
#include "feat.h"

BSDATA(feati) = {
	{"Building"},
	{"Water"},
	{"Discard"},
	{"Shuffle"},
	{"Extend"},
};
assert_enum(feati, Extend)