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
	{"Damage3"},
	{"Damage4"},
	{"Damage6"},
	{"Damage8"},
	{"Damage10"},
	{"Damage12"},
	{"WeaponDice"},
	{"WeaponDiceRaise"},
	{"AC"},
	{"Level"},
	{"HP"},
	{"Rage"},
	{"Dodge"},
};
assert_enum(abilityi, Dodge)