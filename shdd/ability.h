#include "nameable.h"

#pragma once

enum ability_s : unsigned char {
	Strenght, Dexterity, Constitution, Intellect, Wisdow, Charisma,
	MeleeAttack, MeleeDamage,
	RangeAttack, RangeDamage,
	SpellRoll,
	NumberAttacks,
	AC, Level,
	HP, Rage, Dodge
};
struct abilityi : nameable {
};
