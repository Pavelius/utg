#include "creature.h"
#include "monster.h"
#include "script.h"

creature* player;
creature* party[8];

int calculate(statable* p, variants source);

static void set_maximum() {
	for(size_t i = 0; i < sizeof(player->maximum.consumables) / sizeof(player->maximum.consumables[0]); i++)
		player->consumables[i] = player->maximum.consumables[i];
}

void creature::clear() {
	memset(this, 0, sizeof(*this));
}

void creature::create() {
	player = bsdata<creature>::add();
	player->clear();
	player->advance("PlayerGeneration", 0);
	player->update();
	set_maximum();
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
	set_maximum();
}

static void update_maximum() {
	for(size_t i = 0; i < sizeof(player->maximum.consumables) / sizeof(player->maximum.consumables[0]); i++) {
		auto p = bsdata<consumablei>::elements + i;
		if(p->maximum)
			player->maximum.consumables[i] = calculate(player, p->maximum);
	}
}

void creature::update() {
	auto push = player;
	player = this;
	copy(basic);
	statable::update();
	update_maximum();
	player = push;
}