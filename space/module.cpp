#include "crt.h"
#include "module.h"

BSDATA(weapon_typei) = {
	{"Cinetic"},
	{"Beam"},
	{"Rocket"},
};
assert_enum(weapon_typei, Rocket)