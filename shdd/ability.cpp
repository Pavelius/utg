#include "ability.h"
#include "crt.h"

BSDATA(abilityi) = {
	{"Strenght"},
	{"Dexterity"},
	{"Constitution"},
	{"Intellect"},
	{"Wisdow"},
	{"Charisma"},
	{"MeleeAttack"},
	{"MeleeDamage"},
	{"RangeAttack"},
	{"RangeDamage"},
	{"SpellRoll"},
	{"NumberAttacks"},
	{"AC"},
	{"Level"},
	{"HP"},
	{"Rage"},
	{"Dodge"},
};
assert_enum(abilityi, Dodge)