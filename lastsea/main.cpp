#include "bsreq.h"
#include "main.h"

static void generate_crew(bool interactive) {
	auto push_interactive = logs::interactive;
	auto push_header = logs::header;
	logs::interactive = interactive;
	logs::header = getnm("GenerateCrew");
	game.generate();
	bsdata<pirate>::get(0).act(logs::sb, getnm("PromtStart"));
	logs::pause();
	game.choosehistory();
	logs::header = push_header;
	logs::interactive = push_interactive;
}

static void all_gaintreasure() {
	piratea source;
	for(auto p : source)
		p->gaintreasure();
}

static void generate_scenario() {
	game.createtreasure();
	all_gaintreasure();
}

static void initialize_game() {
	game.clear();
	generate_crew(false);
	generate_scenario();
	game.getpirate(0)->setaction(0);
	game.play();
}

static void read_files() {
	bsreq::read("rules/Treasure.txt");
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