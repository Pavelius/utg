#include "crt.h"
#include "modifier.h"

modifier_s modifier;

BSDATA(modifieri) = {
	{"Current"},
	{"Permanent"},
	{"Resist"},
	{"Immune"},
	{"Vulnerable"},
};
assert_enum(modifieri, Vulnerable)