#include "bsreq.h"
#include "strategy.h"
#include "main.h"

void initialize_conditions();

static void main_menu() {
}

static void test_scenario() {
	for(auto& e : bsdata<scenarioi>()) {
		if(!e.tiles[1].type)
			break;
	}
}

static void initialization() {
	bsreq::read("rules/Scenarios.txt");
	test_scenario();
}

void status_info(void) {
}

int main(int argc, char* argv[]) {
	srand(getcputime());
	initialize_conditions();
	return draw::strategy(main_menu, initialization);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}