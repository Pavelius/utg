#include "nameable.h"
#include "flagable.h"

#pragma once

enum feat_s : unsigned char {
	EnergyDrain, Paralysis, PetrifyingGaze, PoisonImmunity, StrenghtDrain,
	SunSensitive, Slow, NormalWeaponImmunity,
	Blunt, Martial, TwoHanded,
	WearLeather, WearIron, WearLarge, WearShield,
	Undead, Summoned, Player, Enemy,
};
struct feati : nameable {
};
struct featable : flagable<4> {};