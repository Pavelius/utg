#include "nameable.h"
#include "flagable.h"

#pragma once

enum feat_s : unsigned char {
	EnergyDrain, Paralysis, PetrifyingGaze, PoisonImmunity, StrenghtDrain,
	Blink, DeathGaze, Defensive, Stampede, Burrowing, BattleRage, StunningBlow, SurpriseEnemy,
	SunSensitive, Slow, Small, Large, Leap, NormalWeaponImmunity,
	Blunt, TwoHanded, Burned, Frozen, Corrosed,
	EngageMelee, Surprised, Prone, Unholy, Summoned, Player, Enemy,
};
struct feati : nameable {
};
struct featable : flagable<8> {};