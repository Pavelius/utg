#include "nameable.h"
#include "flagable.h"

#pragma once

enum featn : unsigned char {
	EnergyDrain, Paralysis, PetrifyingBreath, PetrifyingGaze, PoisonImmunity, CharmImmunity, StrenghtDrain,
	PoisonAttack,
	AcidDamage, Blink, ChargeDouble, DeathGaze, Defensive, Regeneration, Stampede, BloodSucking,
	Burrowing, Cling, Fly, ShapeChanging, InflictWound,
	BattleRage, BearHug, StunningBlow, SurpriseEnemy, DancePoison, Dive,
	SunSensitive, Slow, Quick, Small, Large, Leap, Swoop, NormalWeaponImmunity, DoubleSized,
	ColdResist, FireResist,
	Blunt, TwoHanded, Burned, Cursed, Dark, Frozen, Corrosed, Paniced, ProtectedFromEvil, Trapped, Wounded, Paralized,
	EngageMelee, Blind, Deaf, Surprised, Prone, Unholy, Summoned, Player, Enemy,
};
struct feati : nameable {
};
struct featable : flagable<8> {};