#include "crt.h"
#include "feat.h"

BSDATA(feati) = {
	{"BonusTalent"},
	{"BonusHitRoll"},
	{"Summoned"},
	{"Player"},
	{"Enemy"},
};
assert_enum(feati, Enemy)