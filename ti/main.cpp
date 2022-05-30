#include "bsreq.h"
#include "main.h"
#include "strategy.h"

static char console_text[4096 * 2];
static stringbuilder console(console_text);

static void test_answers() {
	auto p1 = bsdata<actioncardi>::elements + 0;
	auto p2 = bsdata<strategyi>::elements + 1;
	auto p3 = bsdata<playeri>::elements + 0;
	game.prepare();
	game.updateui();
	troop::add("Infantry", p3);
	troop::add("SpaceDock", p3);
	troop::add("Fighter", p3);
	troop::add("Carrier", p3);
	game.active->apply(p2->primary);
}

static void start_game() {
	test_answers();
}

static void initialize() {
	bsreq::read("rules/Planets.txt");
	bsreq::read("rules/ActionCards.txt");
	bsreq::read("rules/Objectives.txt");
	answers::console = &console;
	answers::prompt = console_text;
}

int main(int argc, char* argv[]) {
	srand(getcputime());
	return draw::strategy(start_game, initialize);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}