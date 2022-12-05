#include "nameable.h"

#pragma once

enum wear_s : unsigned char {
	Backpack, Potion, BackpackLast = Backpack + 15,
	MeleeWeapon, MeleeWeaponOffhand, RangedWeapon,
	Head, Torso, Backward, Elbows,
};
struct weari : nameable {
};