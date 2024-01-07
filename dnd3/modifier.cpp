#include "crt.h"
#include "modifier.h"
#include "script.h"

modifier_s modifier;

template<> void fnscript<modifieri>(int index, int value) {
	modifier = modifier_s(index);
}

BSDATA(modifieri) = {
	{"Current"},
	{"Permanent"},
	{"Calculation"},
	{"Resist"},
	{"Immune"},
	{"Vulnerable"},
};
assert_enum(modifieri, Vulnerable)