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
	{"Martial"},
	{"TwoHanded"},
	{"WearLeather"},
	{"WearIron"},
	{"WearLarge"},
	{"WearShield"},
	{"EngageMelee"},
	{"Surprised"},
	{"Prone"},
	{"Undead"},
	{"Summoned"},
	{"Player"},
	{"Enemy"},
};
assert_enum(feati, Enemy)