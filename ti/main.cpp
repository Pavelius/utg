#include "answers.h"
#include "bsreq.h"
#include "draw_strategy.h"
#include "entitya.h"
#include "player.h"
#include "rand.h"
#include "script.h"
#include "system.h"
#include "speech.h"
#include "timer.h"
#include "troop.h"

void initialize_ui();
static char console_text[4096 * 2];
static stringbuilder console(console_text);

#ifdef _DEBUG

void util_main();

static void test_combat() {
	prepare_game();
	update_control();
	auto system = human_player->gethome();
	auto enemy = bsdata<playeri>::find("SardakkNorr");
	if(!enemy)
		return;
	create_troop(bsdata<uniti>::find("Flagman"), enemy, system);
	create_troop(bsdata<uniti>::find("Destroyer"), enemy, system);
	update_ui();
	system->focusing();
	system->startcombat();
}

static void test_script() {
	script_run("TestingScript");
}

#endif // _DEBUG

static void start_game() {
#ifdef _DEBUG
	util_main();
#endif
	//test_combat();
	prepare_game();
	test_script();
	play_game();
}

static void initialize() {
	//speech::read("locale/ru/Speech.txt");
}

int main(int argc, char* argv[]) {
	//srand(getcputime());
	srand(505);
	initialize_ui();
	answers::console = &console;
	answers::prompt = console_text;
	initialize_game();
	return draw::strategy(start_game, initialize);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}