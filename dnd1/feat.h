#include "nameable.h"

#pragma once

enum feat_s : unsigned char {
	EnergyDrain, Paralysis, PetrifyingGaze, PoisonImmunity, StrenghtDrain,
	SunSensitive, Slow, NormalWeaponImmunity,
	Blunt, Martial, TwoHanded,
	WearLeather, WearIron, WearLarge, WearShield, Countable,
	Undead, Summoned, Player, Enemy,
};
struct feati : nameable {
};