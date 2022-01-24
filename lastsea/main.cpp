#include "bsreq.h"
#include "main.h"

static void generate_crew(bool interactive) {
	auto push_interactive = utg::interactive;
	auto push_header = utg::header;
	utg::interactive = interactive;
	utg::header = getnm("GenerateCrew");
	game.generate();
	game.act(utg::sb, getnm("PromtStart"));
	utg::pause();
	game.choosehistory();
	utg::header = push_header;
	utg::interactive = push_interactive;
}

static void all_gaintreasure() {
	game.gaintreasure();
}

static void generate_scenario() {
	game.createtreasure();
	bsdata<quest>::source.clear();
	quest::read("scenario/BeyondTheOceanEdge.txt");
	all_gaintreasure();
}

static void initialize_game() {
	game.clear();
	generate_crew(false);
	generate_scenario();
	quest::run(0, 0, utg::url, 0);
	game.addaction("PortMarket");
	game.play();
}

static void read_files() {
	bsreq::read("rules/Treasure.txt");
	charname::read("locale/ru/PirateNames.txt");
	messagei::read("locale/ru/PirateHistory.txt");
	groupvaluei::read("locale/ru/PirateHistoryVariants.txt");
}

int main(int argc, char* argv[]) {
	utg::url = "pirate_kingship";
	utg::callback::getinfo = game.getpropertyst;
	//srand(getcputime());
	quest::apply = game.applyb;
	return draw::utgx::run(initialize_game, true, read_files);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}