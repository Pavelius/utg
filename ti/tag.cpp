#include "crt.h"
#include "tag.h"

BSDATA(tagi) {
	{"Player1"},
	{"Player2"},
	{"Player3"},
	{"Player4"},
	{"Player5"},
	{"Player6"},
	{"Exhaused"},
	{"NonFighter"},
	{"IgnorePlanetaryShield"},
	{"IgnoreSpaceCannon"},
	{"IgnoreDirectHit"},
	{"PlanetaryShield"},
	{"RepairSustainDamage"},
	{"SustainDamage"},
	{"DeepSpaceCannon"},
	{"CombatBonusToOthers"},
	{"BuyCombatBonusToOther"},
	{"ProduceInfantry"},
	{"HeavyHits"},
	{"AdditionalHitOn9n10"},
	{"AddPlanetResourceToProduction"},
};
assert_enum(tagi, AddPlanetResourceToProduction)