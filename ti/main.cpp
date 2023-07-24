#include "bsreq.h"
#include "draw_strategy.h"
#include "main.h"
#include "speech.h"

void initialize_ui();
static char console_text[4096 * 2];
static stringbuilder console(console_text);

#ifdef _DEBUG
void util_main();

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

static void test_speech() {
	variant test[] = {"Leadership"};
	auto p = speech::find(test);
}

static void test_combat() {
	game.prepare();
	game.updatecontrol();
	auto system = playeri::human->gethome();
	auto enemy = bsdata<playeri>::find("SardakkNorr");
	if(!enemy)
		return;
	troop::create(bsdata<uniti>::find("Flagman"), enemy, system);
	troop::create(bsdata<uniti>::find("Fighter"), enemy, system);
	game.updateui();
	game.focusing(system);
	system->startcombat();
}

static void test_variant() {
	variant v1 = "Capacity";
	variant v2 = bsdata<uniti>::elements + 1;
	auto i1 = (unsigned)v1;
}

#endif // _DEBUG

void tech_selection();

static void new_game() {
	game.prepare();
	tech_selection();
	game.play();
}

static void start_game() {
#ifdef _DEBUG
	util_main();
	test_variant();
#endif // _DEBUG
	new_game();
}

static void initialize() {
	bsreq::read("rules/Planets.txt");
	bsreq::read("rules/ActionCards.txt");
	bsreq::read("rules/Objectives.txt");
	speech::read("locale/ru/Speech.txt");
	answers::console = &console;
	answers::prompt = console_text;
	gamei::initialize();
}

int main(int argc, char* argv[]) {
	//srand(getcputime());
	srand(505);
	initialize_ui();
	return draw::strategy(start_game, initialize);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}