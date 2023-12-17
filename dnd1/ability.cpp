#include "ability.h"
#include "crt.h"

BSDATA(abilityi) = {
	{"Strenght"},
	{"Dexterity"},
	{"Constitution"},
	{"Intellect"},
	{"Wisdow"},
	{"Charisma"},
	{"MeleeToHit"},
	{"MeleeDamage"},
	{"RangedToHit"},
	{"RangedDamage"},
	{"AC"},
	{"ACRanged"},
	{"Level"},
	{"HPMax"},
	{"Speed", 16},
	{"Search", 16},
	{"ClimbWalls", 1},
	{"FindOrRemoveTraps", 1},
	{"HearNoise", 16},
	{"HideInShadows", 1},
	{"MoveSilently", 1},
	{"OpenLocks", 1},
	{"PickPockets", 1},
	{"HideUndergrowth", 1},
	{"SaveDeath", 5},
	{"SaveWands", 5},
	{"SaveParalize", 5},
	{"SaveBreathWeapon", 5},
	{"SaveSpells", 5},
	{"SavePoison", 5},
	{"SpellLevel1"},
	{"SpellLevel2"},
	{"SpellLevel3"},
	{"SpellLevel4"},
	{"SpellLevel5"},
	{"HP"},
	{"IllusionCopies"},
};
assert_enum(abilityi, IllusionCopies)