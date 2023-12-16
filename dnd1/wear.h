#include "nameable.h"

#pragma once

enum wear_s : unsigned char {
	Backpack, Potion, Scroll, BackpackLast = Backpack + 15,
	MeleeWeapon, MeleeWeaponOffhand, RangedWeapon, ThrownWeapon, Ammunition,
	Head, Torso, Legs, Gloves, FingerRight, FingerLeft, Elbows,
};
struct weari : nameable {
};