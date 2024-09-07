#include "bsdata.h"
#include "state.h"

state_s last_state;

BSDATA(statei) = {
	{"Concealment"},
	{"Flatfooted"},
	{"Immobilized"}, 
	{"Prone"},
	{"PermanentConditions"},
	{"Helpless"},
	{"MeleeEngaged"},
};
assert_enum(statei, MeleeEngaged)