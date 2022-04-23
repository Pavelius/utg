#include "main.h"

BSDATA(triggeri) = {
	{"NoTrigger"},
	{"PhysicalWeapon"},
	{"MagicalWeapon"},
	{"Book"},
	{"CombatCheck"},
	{"EvadeCheck"},
	{"HorrorCheck"},
	{"SpellCheck"},
	{"Movement"},
	{"HealthLose"},
	{"SanityLose"},
	{"HealthOrSanityLose"},
};
assert_enum(triggeri, HealthOrSanityLose)