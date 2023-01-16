#include "answers.h"
#include "army.h"
#include "bsreq.h"
#include "building.h"
#include "collection.h"
#include "costitem.h"
#include "draw.h"
#include "draw_object.h"
#include "draw_strategy.h"
#include "game.h"
#include "log.h"
#include "player.h"
#include "province.h"
#include "pushvalue.h"
#include "script.h"

using namespace draw;

void add_neutral(const char* id);
void next_turn();
void ui_initialize();
void update_provinces();
void util_main();
void conquest();

static void test_army() {
	province = bsdata<provincei>::find("NandaDevi");
	auto hero = bsdata<heroi>::find("Gordek");
	hero->province = province;
	hero->player = player;
	for(auto& e : bsdata<troop>()) {
		if(e.player == player)
			e.moveto = province;
	}
	add_neutral("Spearmans");
	add_neutral("Spearmans");
	add_neutral("Spearmans");
	conquest();
}

static void initialize_scene() {
	player = bsdata<playeri>::find("SouthernKindom");
	province = bsdata<provincei>::find("CapeBrumal");
	province->player = player;
	player->upgrade[Lore] += 50;
	script::run(player->start);
	game.initialize();
	update_provinces();
	script::run("UpdatePlayer");
	test_army();
}

static void start_game() {
#ifdef _DEBUG
	util_main();
#endif // _DEBUG
	bsreq::read("maps/silentseas.txt");
	if(log::geterrors())
		return;
	initialize_scene();
	draw::setnext(next_turn);
}

int main(int argc, char* argv[]) {
	srand(getcputime());
	ui_initialize();
	return draw::strategy(start_game, 0);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}