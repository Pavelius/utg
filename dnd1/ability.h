#include "nameable.h"

#pragma once

enum ability_s : unsigned char {
	Strenght, Dexterity, Constitution, Intellect, Wisdow, Charisma,
	MeleeToHit, MeleeDamage, RangedToHit, RangedDamage,
	AC, ACRanged, Level,
	HPMax,
	Speed, Search,
	ClimbWalls, FindOrRemoveTraps, HearNoise, HideInShadows, MoveSilently, OpenLocks, PickPockets,
	HideUndergrowth,
	SaveDeath, SaveWands, SaveParalize, SaveBreathWeapon, SaveSpells, SavePoison,
	SpellLevel1, SpellLevel2, SpellLevel3, SpellLevel4, SpellLevel5,
	HP, IllusionCopies,
};
struct abilityi : nameable {
};
