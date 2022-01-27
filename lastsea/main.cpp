#include "bsreq.h"
#include "drawobject.h"
#include "main.h"
#include "widget.h"

static void createcrew() {
	utg::header = getnm("GenerateCrew");
	game.generate();
	game.choosehistory();
	game.createtreasure();
}

static void starting() {
	utg::interactive = false;
	createcrew();
	utg::interactive = true;
	game.gaintreasure();
	//game.showindecies();
	game.adventure(0);
	game.choose(0);
	utg::pause();
	game.afterapply();
}

static void read_files() {
	bsreq::read("rules/Treasures.txt");
	charname::read("locale/ru/PirateNames.txt");
	messagei::read("locale/ru/PirateHistory.txt");
	groupvaluei::read("locale/ru/PirateHistoryVariants.txt");
	quest::read("scenario/BeyondTheOceanEdge.txt");
	quest::read("locale/ru/QuestActions.txt");
}

void util_main();

int main(int argc, char* argv[]) {
	util_main();
	utg::url = "pirate_kingship";
	utg::callback::getinfo = game.sfgetproperty;
	//srand(getcputime());
	variant::sfapply = game.sfapply;
	quest::console = &utg::sb;
	bsdata<widget>::elements[0].proc = pirate::painttreasure;
	draw::object::initialize();
	return draw::start(starting, true, read_files);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}