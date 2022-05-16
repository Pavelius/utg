#include "bsreq.h"
#include "draw_object.h"
#include "strategy.h"
#include "main.h"

void initialize_conditions();

static void main_menu() {
	draw::camera.x = 0;
	draw::camera.y = 0;
	answers an;
	an.add(main_menu, "Test");
	an.choose("Test");
}

static void test_scenario() {
	auto p = bsdata<scenarioi>::find("BlackBarrows");
	p->prepare(0);
	game.setcamera(p->starts[0]);
	creaturei::add("Tinkerer", p->starts[0]);
	creaturei::add("Brute", p->starts[1]);
}

static void initialization() {
	bsreq::read("rules/Monsters.txt");
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