#include "nameable.h"

#pragma once

enum rangen : unsigned char {
	OneAlly,
	OneEnemy, OneEnemyTouch,
	AllyItem,
	Enviroment, OneObject,
};
struct rangei : nameable {
};
extern rangen range;