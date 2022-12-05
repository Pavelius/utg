#include "crt.h"
#include "state.h"

BSDATA(statei) = {
	{"Concealment"},
	{"Flatfooted"},
	{"Immobilized"}, 
	{"Prone"},
	{"PermanentConditions"},
};
assert_enum(statei, PermanentConditions)