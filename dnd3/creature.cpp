#include "creature.h"
#include "monster.h"
#include "script.h"

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

void creature::create(const char* id) {
	create(bsdata<monsteri>::find(id));
}

void creature::create(const monsteri* pm) {
	if(!pm)
		return;
	player = bsdata<creature>::add();
	player->clear();
	script::run(pm->abilities);
	player->basic.setability("Level", pm->level);
	player->basic.creating();
	player->setavatar(pm->id);
	player->update();
}

void creature::update() {
	copy(basic);
	statable::update();
}