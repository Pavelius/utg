#include "crt.h"
#include "modifier.h"

modifier_s modifier;

BSDATA(modifieri) = {
	{"Proficient"},
	{"Immunity"},
	{"Resist"},
	{"Vulnerable"},
};
assert_enum(modifieri, Vulnerable)