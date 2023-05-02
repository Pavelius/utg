#include "crt.h"
#include "feat.h"

BSDATA(feati) = {
	{"BonusTalent"},
	{"BonusHitRoll"},
	{"Finesse"},
	{"Loading"},
	{"Thrown"},
	{"TwoHanded"},
	{"Versatile"},
	{"Summoned"},
	{"Player"},
	{"Enemy"},
};
assert_enum(feati, Enemy)