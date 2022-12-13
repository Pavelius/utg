#include "crt.h"
#include "modifier.h"

BSDATA(modifieri) = {
	{"NoModifier"},
	{"Inflict"},
	{"Suffer"},
};
assert_enum(modifieri, Suffer)

modifier_s modifier;