#include "nameable.h"

#pragma once

enum wear_s : unsigned char {
	Backpack, Potion, BackpackLast = Backpack + 15,
	MeleeWeapon, MeleeWeaponOffhand, RangedWeapon, Ammunition,
	Head, Torso, Legs, Gloves, FingerRight, FingerLeft, Elbows,
};
struct weari : nameable {
};