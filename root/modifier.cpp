#include "bsdata.h"
#include "modifier.h"
#include "script.h"

BSDATA(modifieri) = {
	{"NoModifier"},
	{"Inflict"},
	{"Suffer"},
};
assert_enum(modifieri, Suffer)

modifier_s modifier;

template<> void fnscript<modifieri>(int index, int bonus) {
	modifier = modifier_s(index);
}