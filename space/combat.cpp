#include "combat.h"
#include "crt.h"

BSDATA(combati) = {
	{"ScouningPhase"},
	{"LongRange"},
	{"MediumRange"},
	{"CloseRange"},
	{"FinalPhase"},
};
assert_enum(combati, FinalPhase)