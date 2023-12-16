#include "crt.h"
#include "feat.h"

BSDATA(feati) = {
	{"EnergyDrain"},
	{"Paralysis"},
	{"PetrifyingGaze"},
	{"PoisonImmunity"},
	{"StrenghtDrain"},
	{"SunSensitive"},
	{"Slow"},
	{"Small"},
	{"Large"},
	{"NormalWeaponImmunity"},
	{"Blunt"},
	{"TwoHanded"},
	{"EngageMelee"},
	{"Surprised"},
	{"Prone"},
	{"Undead"},
	{"Summoned"},
	{"Player"},
	{"Enemy"},
};
assert_enum(feati, Enemy)