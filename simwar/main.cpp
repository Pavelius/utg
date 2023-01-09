#include "answers.h"
#include "building.h"
#include "collection.h"
#include "draw.h"
#include "draw_object.h"
#include "draw_strategy.h"
#include "game.h"
#include "player.h"
#include "province.h"
#include "pushvalue.h"
#include "script.h"
#include "unit.h"

using namespace draw;

void player_turn();
void ui_initialize();
void update_provinces();
void util_main();

static void initialize_scene() {
	player = bsdata<playeri>::find("SouthernKindom");
	province = bsdata<provincei>::find("SnakeForest");
	player->upgrade[Lore] += 50;
	script::run(player->start);
	script::run("UpdatePlayer");
	game.year = 1410;
	update_provinces();
}

static void start_game() {
#ifdef _DEBUG
	util_main();
#endif // _DEBUG
	initialize_scene();
	draw::setnext(player_turn);
}

int main(int argc, char* argv[]) {
	srand(getcputime());
	ui_initialize();
	return draw::strategy(start_game, 0);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}