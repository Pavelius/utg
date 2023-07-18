#include "combat.h"
#include "crt.h"

BSDATA(combati) = {
	{"LongRange"},
	{"MediumRange"},
	{"CloseRange"},
	{"FinalPhase"},
};
assert_enum(combati, FinalPhase)