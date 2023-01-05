#include "draw.h"
#include "draw_object.h"
#include "draw_strategy.h"
#include "player.h"
#include "province.h"

using namespace draw;

void ui_initialize();

static void main_scene() {
}

static void initialize_scene() {
	player = bsdata<playeri>::add();
	player->addcost(Gold, 100);
	player->addcost(Mana, 5);
	player->addcost(Lore, 2);
	player->upkeep.addcost(Mana, 2);
	player->upkeep.addcost(Gold, 10);
	player->upkeep.addcost(Lore, 1);
	player->upgrade.addcost(Lore, 100);
}

static void start_game() {
	initialize_scene();
	draw::scene(main_scene);
}

int main(int argc, char* argv[]) {
	srand(getcputime());
	ui_initialize();
	return draw::strategy(start_game, 0);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}