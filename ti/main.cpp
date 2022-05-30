#include "bsreq.h"
#include "main.h"
#include "strategy.h"

static char console_text[4096 * 2];
static stringbuilder console(console_text);

static void test_answers() {
	auto p1 = bsdata<actioncardi>::elements + 0;
	auto p2 = bsdata<strategyi>::elements + 1;
	game.prepare();
	game.updateui();
	game.active->apply(p2->primary);
}

static void test_combat() {
	troop c1, c2, c3;
	c1.create("Fighter");
	c2.create("Fighter");
	c3.create("Destroyer");
	combat e;
	e.attacker.add(&c1);
	e.attacker.add(&c2);
	e.defender.add(&c3);
	auto r1 = e.attacker.fight(Combat, CombatCount);
	auto r2 = e.defender.fight(AntiFighterBarrage, AntiFighterBarrageCount);
	r2 += e.defender.fight(Combat, CombatCount);
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