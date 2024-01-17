#include "assign.h"
#include "crt.h"
#include "pushvalue.h"
#include "ship.h"

ship* player;

static void update_correction() {
	if(player->modules[Hull] > player->hull)
		player->hull = player->modules[Hull];
	if(player->modules[Shield] > player->shield)
		player->shield = player->modules[Shield];
}

void ship::update() {
	pushvalue push(player, this);
	assign<modulea>(*this, basic);
	update_correction();
}