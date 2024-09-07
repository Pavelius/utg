#include "assign.h"
#include "pushvalue.h"
#include "ship.h"

ship* player;
ship* opponent;

static void check_maximum(short& value, module_s v) {
	short m = player->getmaximum(v);
	if(value > m)
		value = m;
}

void ship::update() {
	pushvalue push(player, this);
	assign<modulea>(*this, basic);
	player->hull = player->getmaximum(Hull);
	player->shield = player->getmaximum(Shield);
}

int	ship::getmaximum(module_s v) const {
	switch(v) {
	case Hull: return modules[v] * 10;
	default: return modules[v];
	}
}