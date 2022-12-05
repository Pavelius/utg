#include "crt.h"
#include "state.h"

BSDATA(statei) = {
	{"Concealment"},
	{"Immobilized"}, 
	{"Prone"},
	{"PermanentConditions"},
};
assert_enum(statei, PermanentConditions)