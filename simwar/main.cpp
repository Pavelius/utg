#include "draw.h"
#include "draw_object.h"
#include "draw_strategy.h"
#include "player.h"
#include "province.h"

using namespace draw;

static void main_scene() {
}

static void initialize_scene() {
	player = bsdata<playeri>::add();
	player->addcost(Gold, 100);
	player->addcost(Stone, 5);
	player->addcost(Woods, 5);
	player->upkeep.addcost(Woods, 2);
	player->upkeep.addcost(Gold, 10);
}

static void start_game() {
	initialize_scene();
	draw::scene(main_scene);
}

int main(int argc, char* argv[]) {
	srand(getcputime());
	return draw::strategy(start_game, 0);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}