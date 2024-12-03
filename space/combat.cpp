#include "combat.h"
#include "bsdata.h"

BSDATA(combati) = {
	{"LongRange"},
	{"MediumRange"},
	{"CloseRange"},
	{"FinalPhase"},
};
assert_enum(combati, FinalPhase)