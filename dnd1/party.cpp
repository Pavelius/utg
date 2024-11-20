#include "bsdata.h"
#include "party.h"
#include "script.h"

BSDATA(partystati) = {
	{"EffectCount"},
};
assert_enum(partystati, EffectCount)

partyi party;

template<> void fnscript<partystati>(int index, int value) {
	party.abilities[index] += value;
}