#include "crt.h"
#include "modifier.h"
#include "script.h"

unsigned apply_modifiers;
modifier_s modifier;

BSDATA(modifieri) = {
	{"NoModifier"},
	{"Permanent"},
	{"Enviroment"},
};
assert_enum(modifieri, Enviroment)

template<> void fnscript<modifieri>(int index, int bonus) {
	modifier = (modifier_s)index;
}