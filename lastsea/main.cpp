#include "bsreq.h"
#include "draw_hexagon.h"
#include "main.h"
#include "widget.h"

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
	const int size = 42;
	draw::camera = {200, 100};
	for(short x = 0; x < 7; x++) {
		for(short y = 0; y < 6; y++) {
			auto pt = draw::fh2p({x, y}, size);
			auto p = draw::addobject(pt.x, pt.y);
			p->size = size;
			p->proc = draw::fhexagon;
			p->setcolorborder();
		}
	}
	draw::chooseobject();
}

static void starting() {
	utg::interactive = false;
	createcrew();
	utg::interactive = true;
	game.gaintreasure();
	test_scene();
	//game.adventure(0);
	//utg::pause();
	//game.afterapply();
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
	bsdata<widget>::elements[0].proc = pirate::painttreasure;
	return draw::start(starting, true, read_files);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}