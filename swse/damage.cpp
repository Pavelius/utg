#include "crt.h"
#include "damage.h"

BSDATA(damagei) = {
	{"Bludgeon"},
	{"Slashing"},
	{"Piercing"},
	{"Energy"},
	{"Ion"},
};
assert_enum(damagei, Ion)