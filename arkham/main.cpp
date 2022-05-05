#include "bsreq.h"
#include "draw_object.h"
#include "main.h"
#include "strategy.h"

#ifdef _DEBUG

void util_main();

static void test_movement() {
	answers an;
	an.header = "Перемещение";
	for(auto& e : bsdata<locationi>())
		an.add(&e, getnm(e.id));
	auto p1 = (locationi*)an.open("Отмена");
	if(p1) {
		auto p2 = draw::findobject(&bsdata<character>::get(0));
		p2->move(p1->position, 4);
	}
}

static void test_encounter() {
	game.encounter(1000);
}

#endif // _DEBUG

void update_board();

static void create_deck() {
	for(auto& e : bsdata<cardtypei>()) {
		e.cards.create((cardtype_s)(&e - bsdata<cardtypei>::elements));
		e.cards.shuffle();
	}
}

void main_menu() {
	update_board();
	test_encounter();
}

static void initialization() {
	bsreq::read("rules/Worldmap.txt");
	bsreq::read("rules/Items.txt");
	bsreq::read("rules/Monsters.txt");
	quest::read("locale/ru/Encounters.txt");
}

int main(int argc, char* argv[]) {
	srand(getcputime());
#ifdef _DEBUG
	util_main();
#endif // _DEBUG
	draw::object::initialize();
	return draw::strategy(main_menu, initialization);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}