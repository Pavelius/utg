#include "crt.h"
#include "feat.h"

BSDATA(feati) = {
	{"EnergyDrain"},
	{"Paralysis"},
	{"PetrifyingGaze"},
	{"PoisonImmunity"},
	{"StrenghtDrain"},
	{"Stampede"},
	{"SunSensitive"},
	{"Slow"},
	{"Small"},
	{"Large"},
	{"NormalWeaponImmunity"},
	{"Blunt"},
	{"TwoHanded"},
	{"Burned"},
	{"Frozen"},
	{"EngageMelee"},
	{"Surprised"},
	{"Prone"},
	{"Unholy"},
	{"Summoned"},
	{"Player"},
	{"Enemy"},
};
assert_enum(feati, Enemy)