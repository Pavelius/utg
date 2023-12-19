#include "nameable.h"

#pragma once

enum range_s : unsigned char {
	Caster, CasterOrAlly, OneAlly, OneEnemy, SomeEnemies,
	AllAlly, AllEnemies,
	OneItem, OneAllyItem, AllCasterItems, AllAllyItems,
	Enviroment, OneObject, OneRandomObject
};
struct rangei : nameable {
};
extern range_s range;