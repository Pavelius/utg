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
	{"NormalWeaponImmunity"},
	{"Blunt"},
	{"Martial"},
	{"TwoHanded"},
	{"WearLeather"},
	{"WearIron"},
	{"WearLarge"},
	{"WearShield"},
	{"Undead"},
	{"Summoned"},
	{"Player"},
	{"Enemy"},
};
assert_enum(feati, Enemy)