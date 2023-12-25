#include "nameable.h"
#include "flagable.h"

#pragma once

enum feat_s : unsigned char {
	EnergyDrain, Paralysis, PetrifyingGaze, PoisonImmunity, StrenghtDrain,
	Blink, Defensive, Stampede, Burrowing, BattleRage,
	SunSensitive, Slow, Small, Large, NormalWeaponImmunity,
	Blunt, TwoHanded, Burned, Frozen, Corrosed,
	EngageMelee, Surprised, Prone, Unholy, Summoned, Player, Enemy,
};
struct feati : nameable {
};
struct featable : flagable<8> {};