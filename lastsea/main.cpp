#include "charname.h"
#include "main.h"

static void generate_crew() {
	auto push_header = logs::header;
	logs::header = getnm("GenerateCrew");
	game.generate();
	bsdata<pirate>::get(0).act(logs::sb, getnm("PromtStart"));
	logs::pause();
	game.choosehistory();
	logs::header = push_header;
}

static void initialize_game() {
	generate_crew();
}

static void read_files() {
	charname::read("locale/ru/PirateNames.txt");
	messagei::read("locale/ru/PirateHistory.txt");
	groupvaluei::read("locale/ru/PirateHistoryVariants.txt");
}

int main(int argc, char* argv[]) {
	draw::heroes = bsdata<pirate>::source_ptr;
	draw::heroes_getavatar = pirate::getavatarst;
	logs::url = "pirate_kingship";
	//srand(getcputime());
	return draw::utg::run(initialize_game, true, read_files);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}