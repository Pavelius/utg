#include "bsreq.h"
#include "main.h"

static void createcrew() {
	utg::header = getnm("GenerateCrew");
	game.clear();
	game.generate();
	game.choosehistory();
	game.createtreasure();
}

static void test_scene() {
	draw::object::initialize();
	draw::clearobjects();
	auto p1 = draw::addobject(10, 10);
	p1->setcolorborder();
	p1->string = "Text string 1";
	p1->resource = draw::gres("small_seamap", "art/background", {});
	auto p2 = draw::addobject(200, 200);
	p2->border = figure::Circle;
	p2->string = "Text string 2";
	draw::chooseobject();
}

static void starting() {
	utg::interactive = false;
	createcrew();
	utg::interactive = true;
	game.gaintreasure();
	game.adventure(0);
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

int main(int argc, char* argv[]) {
	utg::url = "pirate_kingship";
	utg::callback::getinfo = game.sfgetproperty;
	//srand(getcputime());
	variant::sfapply = game.sfapply;
	quest::console = &utg::sb;
	return draw::start(starting, true, read_files);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}