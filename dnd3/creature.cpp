#include "creature.h"
#include "monster.h"
#include "script.h"

creature* player;
creature* party[8];

void creature::clear() {
	memset(this, 0, sizeof(*this));
}

void creature::create() {
	player = bsdata<creature>::add();
	player->clear();
	player->advance("PlayerGeneration", 0);
}

void creature::create(const char* id) {
	create(bsdata<monsteri>::find(id));
}

void creature::create(const monsteri* pm) {
	if(!pm)
		return;
	player = bsdata<creature>::add();
	player->clear();
	script_run(pm->abilities);
	player->basic.setability("Level", pm->level);
	player->basic.creating();
	player->setavatar(pm->id);
	player->update();
}

void creature::update() {
	copy(basic);
	statable::update();
}