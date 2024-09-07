#include "bsdata.h"
#include "forcepower.h"

BSDATA(forcetypei) = {
	{"CommonForce"},
	{"Telekinetic"},
	{"DarkSide"},
	{"LightSide"},
};
assert_enum(forcetypei, LightSide)