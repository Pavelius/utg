#include "bsreq.h"
#include "draw_object.h"
#include "strategy.h"
#include "main.h"

void initialize_conditions();

#ifdef _DEBUG
void util_main();
#endif // _DEBUG

static void play_game() {
	while(true)
		game.playround();
}

static void test_scenario() {
	auto p = bsdata<scenarioi>::find("BlackBarrows");
	p->prepare(0);
	auto p1 = bsdata<playeri>::find("Tinkerer");
	auto p2 = bsdata<playeri>::find("Brute");
	p1->combat.initialize();
	p2->combat.initialize();
	p1->hand.addcards(p1->id, 1);
	p2->hand.addcards(p2->id, 1);
	game.combat.initialize();
	auto c2 = creaturei::add("Brute", p->starts[1]);
	auto c1 = creaturei::add("Tinkerer", p->starts[0]);
	play_game();
}

void ui_initialize();

static void main_menu() {
	ui_initialize();
	test_scenario();
}

static void initialization() {
	game.initialize();
	bsreq::read("rules/CombatCards.txt");
	bsreq::read("rules/Monsters.txt");
	bsreq::read("rules/Scenarios.txt");
}

void status_info(void) {
}

int main(int argc, char* argv[]) {
#ifdef _DEBUG
	util_main();
#endif // _DEBUG
	srand(getcputime());
	initialize_conditions();
	return draw::strategy(main_menu, initialization);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}