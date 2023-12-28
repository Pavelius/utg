#include "nameable.h"
#include "flagable.h"

#pragma once

enum feat_s : unsigned char {
	EnergyDrain, Paralysis, PetrifyingBreath, PetrifyingGaze, PoisonImmunity, CharmImmunity, StrenghtDrain,
	AcidDamage, Blink, ChargeDouble, DeathGaze, Defensive, Stampede, BloodSucking,
	Burrowing, Cling, Fly, ShapeChanging, InflictWound,
	BattleRage, BearHug, StunningBlow, SurpriseEnemy, DancePoison, Dive,
	SunSensitive, Slow, Quick, Small, Large, Leap, Swoop, NormalWeaponImmunity,
	Blunt, TwoHanded, Burned, Frozen, Corrosed, Wounded,
	EngageMelee, Surprised, Prone, Unholy, Summoned, Player, Enemy,
};
struct feati : nameable {
};
struct featable : flagable<8> {};