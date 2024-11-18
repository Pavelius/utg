#include "nameable.h"

#pragma once

enum rangen : unsigned char {
	OneAlly,
	OneEnemy, OneEnemyTouch,
	AllyItem,
	Enviroment, Scenery,
};
struct rangei : nameable {
};
extern rangen range;