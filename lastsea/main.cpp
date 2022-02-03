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

void start_scene();

static void starting() {
	answers::interactive = false;
	answers::header = getnm("GenerateCrew");
	game.generate();
	game.choosehistory();
	answers::interactive = true;
	game.createtreasure();
	game.createtiles();
	game.script(0);
	draw::setnext(start_scene);
	//game.script(826);
}

static void initializing() {
	bsreq::read("rules/Treasures.txt");
	charname::read("locale/ru/PirateNames.txt");
	messagei::read("locale/ru/PirateHistory.txt");
	groupvaluei::read("locale/ru/PirateHistoryVariants.txt");
	quest::read("scenario/BeyondTheOceanEdge.txt");
	quest::read("locale/ru/QuestActions.txt");
}

static void initialize_widgets() {
	widget::add("MapOfTheSeas", widget::button, gamei::showseamap);
}

static void test_handlers() {
}

void initialize_information_widgets();
void util_main();

int main(int argc, char* argv[]) {
	util_main();
	test_handlers();
	answers::resid = "pirate_kingship";
	utg::callback::getinfo = game.sfgetproperty;
	utg::callback::getstatus = game.sfgetstatus;
	//srand(getcputime());
	variant::sfapply = game.sfapply;
	answers::console = &utg::sb;
	draw::object::initialize();
	oceani::initialize();
	initialize_widgets();
	initialize_information_widgets();
	return draw::start(starting, true, initializing);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}