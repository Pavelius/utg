#include "bsreq.h"
#include "main.h"
#include "strategy.h"

static char console_text[4096 * 2];
static stringbuilder console(console_text);

#ifdef _DEBUG
void util_main();
#endif // _DEBUG

static void test_movement() {
	auto p1 = bsdata<troop>::elements + 1;
	auto p2 = bsdata<troop>::elements + 4;
	auto p3 = bsdata<troop>::elements + 5;
	auto n1 = bsdata<planeti>::elements + 1;
	p2->location = p1;
	p3->location = p1;
	game.updateui();
	draw::output("Отряды поднялись на борт"); draw::pause();
	p1->location = bsdata<systemi>::elements + 1;
	game.updateui();
	draw::output("Крейсер передвинулся"); draw::pause();
	p2->location = n1;
	p3->location = n1;
	game.updateui();
	draw::output("Отряды высадились на планету"); draw::pause();
}

static void test_answers() {
	game.prepare();
	game.updateui();
	auto p2 = bsdata<strategyi>::elements + 1;
	game.active->apply(p2->primary);
	test_movement();
}

static void start_game() {
#ifdef _DEBUG
	util_main();
#endif // _DEBUG
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