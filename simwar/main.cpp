#include "answers.h"
#include "army.h"
#include "bsreq.h"
#include "site.h"
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
#include "stringbuilder.h"
#include "script.h"
#include "rand.h"
#include "timer.h"

using namespace draw;

void add_neutral(const char* id);
void next_turn();
void show_messages();
void ui_initialize();
void util_main();
void initialize_relation();

static void initialize_scene() {
	player = bsdata<playeri>::find("SouthernKindom");
	province = bsdata<provincei>::find("CapeBrumal");
	province->player = player;
	script_run(player->start);
	initialize_game();
	script_run("UpdatePlayer");
}

static void start_game() {
#ifdef _DEBUG
	util_main();
#endif // _DEBUG
	bsreq::read("maps/silentseas.txt");
	if(log::errors > 0)
		return;
	if(game.read("autosave"))
		draw::setnext(show_messages);
	else {
		initialize_scene();
		draw::setnext(next_turn);
	}
}

int main(int argc, char* argv[]) {
	srand(getcputime());
	// srand(10293);
	ui_initialize();
	initialize_relation();
	return draw::strategy(start_game, 0);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}