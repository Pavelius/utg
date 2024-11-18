#include "nameable.h"
#include "flagable.h"

#pragma once

enum feat_s : unsigned char {
	EnergyDrain, Paralysis, PetrifyingBreath, PetrifyingGaze, PoisonImmunity, CharmImmunity, StrenghtDrain,
	AcidDamage, Blink, ChargeDouble, DeathGaze, Defensive, Regeneration, Stampede, BloodSucking,
	Burrowing, Cling, Fly, ShapeChanging, InflictWound,
	BattleRage, BearHug, StunningBlow, SurpriseEnemy, DancePoison, Dive,
	SunSensitive, Slow, Quick, Small, Large, Leap, Swoop, NormalWeaponImmunity,
	ColdResist, FireResist,
	Blunt, TwoHanded, Burned, Dark, Frozen, Corrosed, Paniced, ProtectedFromEvil, Trapped, Wounded, Paralized,
	EngageMelee, Blind, Deaf, Surprised, Prone, Unholy, Summoned, Player, Enemy,
};
struct feati : nameable {
};
struct featable : flagable<8> {};