#include "bsreq.h"
#include "main.h"

static void generate_crew(bool interactive) {
	auto push_interactive = utg::interactive;
	auto push_header = utg::header;
	utg::interactive = interactive;
	utg::header = getnm("GenerateCrew");
	game.clear();
	game.generate();
	game.choosehistory();
	game.createtreasure();
	utg::header = push_header;
	utg::interactive = push_interactive;
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

static void initialize_game() {
	generate_crew(false);
	game.gaintreasure();
	game.adventure(0);
	utg::pause();
	//game.addaction("PortMarket");
	game.play();
}

static void read_files() {
	bsreq::read("rules/Treasure.txt");
	charname::read("locale/ru/PirateNames.txt");
	messagei::read("locale/ru/PirateHistory.txt");
	groupvaluei::read("locale/ru/PirateHistoryVariants.txt");
	quest::read("scenario/BeyondTheOceanEdge.txt");
}

int main(int argc, char* argv[]) {
	utg::url = "pirate_kingship";
	utg::callback::getinfo = game.sfgetproperty;
	//srand(getcputime());
	variant::sfapply = game.sfapply;
	quest::console = &utg::sb;
	return draw::utgx::run(initialize_game, true, read_files);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}