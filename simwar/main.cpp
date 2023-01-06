#include "building.h"
#include "draw.h"
#include "draw_object.h"
#include "draw_strategy.h"
#include "player.h"
#include "province.h"
#include "script.h"

using namespace draw;

void ui_initialize();

static void main_scene() {
}

static void initialize_scene() {
	player = bsdata<playeri>::find("SouthernKindom");
	province = bsdata<provincei>::find("SnakeForest");
	script::run(player->start);
	script::run("UpdatePlayer");
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