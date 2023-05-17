#include "creature.h"

void creature::clear() {
	memset(this, 0, sizeof(*this));
}

void creature::create() {
	clear();
	abilities[0] = 14;
	abilities[1] = 13;
	abilities[2] = 13;
	abilities[3] = 13;
	abilities[4] = 13;
	abilities[5] = 13;
}