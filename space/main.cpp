#include "bsreq.h"
#include "log.h"
#include "main.h"
#include "quest.h"
#include "script.h"
#include "strategy.h"

void initialize_interface();

static void generate_systems() {
	for(auto& e : bsdata<systemi>())
		e.setposition({400, 300});
}

static void generate_planets() {
	const int dx = 40;
	const int dn = 6;
	for(auto& e : bsdata<planeti>()) {
		point pt;
		auto index = rand() % (16 * 10);
		pt.x = (index % 16) * dx + dn + rand() % (dx - dn * 2);
		pt.y = (index / 10) * dx + dn + rand() % (dx - dn * 2);
		e.setposition(pt);
	}
}

static void test_game() {
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