#include "bsdata.h"
#include "modifier.h"

modifier_s modifier;

BSDATA(modifieri) = {
	{"NoModifier"},
	{"Permanent"},
	{"ItemProficiency"},
};
assert_enum(modifieri, ItemProficiency)