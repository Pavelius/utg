#include "answers.h"
#include "bsreq.h"
#include "draw.h"
#include "draw_strategy.h"
#include "game.h"
#include "log.h"
#include "planet.h"
#include "quest.h"
#include "questlist.h"
#include "rand.h"
#include "script.h"
#include "ship.h"
#include "timer.h"

static char console[512];
static stringbuilder sb_console(console);

void initialize_interface();
void stringbuilder_proc(stringbuilder& sb, const char* id);

static point getrandom(int index) {
	point pt;
	const int dx = 200;
	const int dy = 150;
	pt.x = dx / 2 + (index % 3) * dx + rand() % dx;
	pt.y = dy / 2 + (index / 3) * dy + rand() % dy;
	return pt;
}

static void generate_planets() {
	const int dx = 200;
	const int dy = 150;
	char seeds[] = {0, 1, 2, 3, 5, 6, 7, 8};
	zshuffle(seeds, sizeof(seeds) / sizeof(seeds[0]));
	auto seed_index = 0;
	for(auto& e : bsdata<planeti>()) {
		e.priority = 11;
		e.position = getrandom(seeds[seed_index++]);
	}
}

static void generate_systems() {
	for(auto& e : bsdata<systemi>()) {
		e.priority = 1;
		e.position = {400, 300};
	}
	last_system = bsdata<systemi>::elements;
}

static void test_game() {
	game.setdate(3000, 4, 15);
	generate_systems();
	generate_planets();
	script_run("StartScript");
	player->add(Insight, 1);
	quest_run(1);
	draw::setnext(play_player_turn);
	draw::start();
}

static void game_initializa() {
	//log::readlocfolder(questlist_read, "actions", "*.txt");
}

int main(int argc, char* argv[]) {
	srand(getcputime());
	quest_initialize();
	answers::console = &sb_console;
	answers::prompt = console;
	stringbuilder::custom = stringbuilder_proc;
	initialize_interface();
	return draw::strategy(test_game, game_initializa);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}