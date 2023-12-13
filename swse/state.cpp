#include "crt.h"
#include "state.h"

state_s last_state;

BSDATA(statei) = {
	{"Concealment"},
	{"Flatfooted"},
	{"Immobilized"}, 
	{"Prone"},
	{"PermanentConditions"},
};
assert_enum(statei, PermanentConditions)