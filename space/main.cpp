#include "bsreq.h"
#include "log.h"
#include "main.h"
#include "quest.h"
#include "script.h"
#include "strategy.h"

void initialize_interface();

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
	for(auto& e : bsdata<planeti>())
		e.setposition(getrandom(seeds[seed_index++]));
}

static void generate_systems() {
	for(auto& e : bsdata<systemi>())
		e.setposition({400, 300});
}

static void test_game() {
	game.setdate(3000, 4, 15);
	auto year = game.getyear();
	auto month = game.getmonth();
	auto day = game.getmonthday();
	generate_systems();
	generate_planets();
	answers an;
	an.choose(0, getnm("Cancel"), 1);
}

static void initialize() {
	bsreq::read("rules/Galaxy.txt");
}

int main(int argc, char* argv[]) {
	srand(getcputime());
	quest::initialize();
	initialize_interface();
	return draw::strategy(test_game, initialize);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}