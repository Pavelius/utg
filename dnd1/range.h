#include "nameable.h"

#pragma once

enum range_s : unsigned char {
	Caster, CasterOrAlly, OneEnemy, SomeEnemies,
	AllAlly, AllEnemies,
	OneItem, OneRandomItem, AllCasterItems, AllPartyItems,
	Enviroment, OneObject, OneRandomObject,
	EncounterReaction,
};
struct rangei : nameable {
};
extern range_s range;