#include "bsdata.h"
#include "unit_type.h"

BSDATA(unit_typei) = {
	{"GroundForces"},
	{"Ships"},
	{"Structures"},
};
assert_enum(unit_typei, Structures)