#include "nameable.h"

#pragma once

enum ability_s : unsigned char {
	Strenght, Dexterity, Constitution, Intellect, Wisdow, Charisma,
	MeleeAttack, MeleeDamage,
	RangeAttack, RangeDamage,
	SpellRoll,
	NumberAttacks,
	Damage3, Damage4, Damage6, Damage8, Damage10, Damage12, WeaponDice,
	AC, Level,
	HP, Rage, Dodge,
};
struct abilityi : nameable {
};