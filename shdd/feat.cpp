#include "bsdata.h"
#include "feat.h"

BSDATA(feati) = {
	{"Coins"},
	{"Ambush"},
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