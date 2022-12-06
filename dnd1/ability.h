#include "nameable.h"

#pragma once

enum ability_s : unsigned char {
	Strenght, Dexterity, Constitution, Intellect, Wisdow, Charisma,
	ToHit, Damage, MeleeToHit, MeleeDamage, RangedToHit, RangedDamage,
	AC, Level,
	HPMax,
	Speed, Search,
	ClimbWalls, FindOrRemoveTraps, HearNoise, HideInShadows, MoveSilently, OpenLocks, PickPockets,
	Saves, SaveDeath, SaveWands, SaveParalize, SaveBreathWeapon, SaveSpells, SavePoison,
	HP, IllusionCopies,
};
struct abilityi : nameable {
};
