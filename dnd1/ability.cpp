#include "ability.h"
#include "crt.h"

BSDATA(abilityi) = {
	{"Strenght"},
	{"Dexterity"},
	{"Constitution"},
	{"Intellect"},
	{"Wisdow"},
	{"Charisma"},
	{"THAC0"},
	{"Damage"},
	{"MeleeToHit"},
	{"MeleeDamage"},
	{"RangedToHit"},
	{"RangedDamage"},
	{"AC"},
	{"Level"},
	{"HPMax"},
	{"Speed"},
	{"Search"},
	{"ClimbWalls"},
	{"FindOrRemoveTraps"},
	{"HearNoise"},
	{"HideInShadows"},
	{"MoveSilently"},
	{"OpenLocks"},
	{"PickPockets"},
	{"Saves"},
	{"SaveDeath"},
	{"SaveWands"},
	{"SaveParalize"},
	{"SaveBreathWeapon"},
	{"SaveSpells"},
	{"SavePoison"},
	{"HP"},
	{"IllusionCopies"},
};
assert_enum(abilityi, IllusionCopies)