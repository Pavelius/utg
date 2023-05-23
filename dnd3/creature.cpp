#include "creature.h"

creature* player;

void creature::clear() {
	memset(this, 0, sizeof(*this));
}

void creature::create() {
	clear();
	basic.abilities[0] = 14;
	basic.abilities[1] = 13;
	basic.abilities[2] = 13;
	basic.abilities[3] = 13;
	basic.abilities[4] = 13;
	basic.abilities[5] = 13;
}

void creature::update() {
	copy(basic);
	statable::update();
}