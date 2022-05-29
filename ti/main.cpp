#include "bsreq.h"
#include "main.h"
#include "strategy.h"

static void start_game() {
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

static void reading() {
	bsreq::read("rules/Planets.txt");
}

int main(int argc, char* argv[]) {
	srand(getcputime());
	return draw::strategy(start_game, reading);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}