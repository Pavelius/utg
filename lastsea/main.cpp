#include "bsreq.h"
#include "draw_object.h"
#include "main.h"
#include "log.h"
#include "recordset.h"
#include "widget.h"

// TODO: Ошибка - нельзя зайти в пестрые пещеры
// TODO: Сделать сохранение.
// TODO: Переделать цели

#ifdef _DEBUG

void util_main();

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

static void test_handlers() {
	recordset source;
	source.select(bsdata<abilityi>::source);
	for(auto p : source.records<abilityi>()) {
		if(!p->id)
			break;
	}
}

#endif // _DEBUG

void start_scene();

void main_menu() {
	quest::manual(6600);
}

static bool load_quest() {
	quest::read("scenario/BeyondTheOceanEdge.txt");
	if(log::geterrors() != 0)
		return false;
	game.script(0);
	menu::current_mode = "Game";
	if(!draw::isnext())
		draw::setnext(start_scene);
	return true;
}

static void starting() {
	answers::interactive = true;
	game.generate();
	game.createtreasure();
	game.createtiles();
	game.script(0);
	answers::interactive = true;
	load_quest();
}

static void initializing() {
	bsreq::read("rules/Treasures.txt");
	bsreq::read("rules/Pirates.txt");
	charname::read("locale/ru/PirateNames.txt");
	messagei::read("locale/ru/PirateHistory.txt");
	groupvaluei::read("locale/ru/PirateHistoryVariants.txt");
	quest::read("locale/ru/QuestActions.txt");
}

void initialize_information_widgets();

void initialize_script();

static void initialize_widgets() {
	widget::add("MapOfTheSeas", widget::button, gamei::showseamap);
}

int main(int argc, char* argv[]) {
#ifdef _DEBUG
	//util_main();
	test_handlers();
#endif // _DEBUG
	answers::resid = "pirate_kingship";
	utg::callback::getinfo = game.sfgetproperty;
	utg::callback::getstatus = game.sfgetstatus;
	srand(getcputime());
	answers::console = &utg::sb;
	answers::prompt = utg::sb.begin();
	draw::object::initialize();
	oceani::initialize();
	quest::initialize();
	goal::initialize();
	goal::info = game.information;
	initialize_script();
	initialize_widgets();
	initialize_information_widgets();
	return draw::start(starting, true, initializing);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}