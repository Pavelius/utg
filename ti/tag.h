#include "flagable.h"
#include "nameable.h"

#pragma once

enum tag_s : unsigned char {
	Player1, Player2, Player3, Player4, Player5, Player6, Exhaust,
	NonFighter,
	IgnorePlanetaryShield, IgnoreSpaceCannon, IgnoreDirectHit, PlanetaryShield, RepairSustainDamage, SustainDamage,
	DeepSpaceCannon,
	CombatBonusToOthers, BuyCombatBonusToOther, ProduceInfantry, HeavyHits, AdditionalHitOn9n10,
	AddPlanetResourceToProduction,
};
struct tagi : nameable {
};
typedef flagable<4> taga;