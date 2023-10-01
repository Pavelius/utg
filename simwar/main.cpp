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
#include "stringact.h"
#include "stringbuilder.h"
#include "script.h"

using namespace draw;

void add_neutral(const char* id);
void next_turn();
void show_messages();
void ui_initialize();
void util_main();
void conquest();

static void test_army() {
	province = bsdata<provincei>::find("NandaDevi");
	auto hero = bsdata<heroi>::find("Gordek");
	hero->province = province;
	hero->player = player;
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
	script_run(player->start);
	game.initialize();
	script_run("UpdatePlayer");
}

static void start_game() {
#ifdef _DEBUG
	util_main();
#endif // _DEBUG
	bsreq::read("maps/silentseas.txt");
	if(log::geterrors())
		return;
	if(game.read("autosave"))
		draw::setnext(show_messages);
	else {
		initialize_scene();
		draw::setnext(next_turn);
	}
}

void initialize_game() {
	stringbuilder::custom = act_identifier;
}

int main(int argc, char* argv[]) {
	srand(getcputime());
	ui_initialize();
	initialize_game();
	return draw::strategy(start_game, 0);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}