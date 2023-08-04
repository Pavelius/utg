#include "bsreq.h"
#include "draw_strategy.h"
#include "main.h"
#include "speech.h"

void initialize_ui();
static char console_text[4096 * 2];
static stringbuilder console(console_text);

#ifdef _DEBUG

void util_main();

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
	troop::create(bsdata<uniti>::find("Destroyer"), enemy, system);
	game.updateui();
	game.focusing(system);
	system->startcombat();
}

void tech_selection();

#endif // _DEBUG


static void new_game() {
	test_combat();
	game.prepare();
	game.play();
}

static void start_game() {
#ifdef _DEBUG
	util_main();
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