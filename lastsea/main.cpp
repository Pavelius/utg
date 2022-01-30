#include "bsreq.h"
#include "drawobject.h"
#include "main.h"
#include "widget.h"

static void test_direction() {
	auto i0 = oceani::to(11, 0);
	auto i1 = oceani::to(11, 1);
	auto i2 = oceani::to(11, 2);
	auto i3 = oceani::to(11, 3);
	auto i4 = oceani::to(11, 4);
	auto i5 = oceani::to(11, 5);
	auto e0 = oceani::to(10, 0);
	auto e1 = oceani::to(10, 1);
	auto e2 = oceani::to(10, 2);
	auto e3 = oceani::to(10, 3);
	auto e4 = oceani::to(10, 4);
	auto e5 = oceani::to(10, 5);
	auto pt = oceani::gethexsize(); // 84 x 72
}

static void starting() {
	utg::interactive = false;
	utg::header = getnm("GenerateCrew");
	game.generate();
	game.choosehistory();
	utg::interactive = true;
	game.createtreasure();
	game.createtiles();
	game.adventure(0);
	game.gaintreasure();
	utg::pause();
	game.adventure(23);
	game.afterapply();
	//game.chartacourse(3);
	//game.chooseroute(0);
	//game.endscene(19);
}

static void initializing() {
	bsreq::read("rules/Treasures.txt");
	charname::read("locale/ru/PirateNames.txt");
	messagei::read("locale/ru/PirateHistory.txt");
	groupvaluei::read("locale/ru/PirateHistoryVariants.txt");
	quest::read("scenario/BeyondTheOceanEdge.txt");
	quest::read("locale/ru/QuestActions.txt");
}

void util_main();

static void initialize_widgets() {
	widget::add("ListOfGoals", shiplog::listofgoals);
	widget::add("ListOfRecords", shiplog::listofrecords);
	widget::add("ListOfTreasures", pirate::painttreasure);
	widget::add("ListOfCounters", gamei::listofcounters);
	widget::add("MapOfTheSeas", widget::button, gamei::showseamap);
}

int main(int argc, char* argv[]) {
	util_main();
	utg::url = "pirate_kingship";
	utg::callback::getinfo = game.sfgetproperty;
	utg::callback::getstatus = game.sfgetstatus;
	//srand(getcputime());
	variant::sfapply = game.sfapply;
	quest::console = &utg::sb;
	draw::object::initialize();
	oceani::initialize();
	initialize_widgets();
	return draw::start(starting, true, initializing);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}