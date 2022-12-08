#include "crt.h"
#include "modifier.h"

modifier_s modifier;
bool permanent_modifier;

BSDATA(modifieri) = {
	{"Temporary"},
	{"Permanent"},
	{"Allowed"},
	{"Proficient"},
	{"Immunity"},
	{"Resist"},
	{"Vulnerable"},
};
assert_enum(modifieri, Vulnerable)