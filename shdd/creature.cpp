#include "creature.h"

creature* player;

static void update_stats() {
	player->load(player->basic);
}

void creature::update() {
	auto push_player = player; player = this;
	update_stats();
	player = push_player;
}