#include "bsreq.h"
#include "draw_object.h"
#include "main.h"
#include "strategy.h"

#ifdef _DEBUG

void util_main();

static void test_encounter() {
	game.add(Speed, 3);
	game.add(Sneak, 3);
	game.add(Fight, 5);
	game.add(Will, 2);
	game.add(Lore, 3);
	game.add(Luck, 0);
	//
	game.pick(Gate);
	game.add(Clue, 3);
	game.movement(locationi::find("TheWitchHouse"));
	game.encounter();
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
	create_deck();
	update_board();
	test_encounter();
}

static void read_quests() {
	for(auto& e : bsdata<locationi>()) {
		if(e.type == Street)
			continue;
		auto pb = bsdata<quest>::end();
		char temp[260]; stringbuilder sb(temp);
		sb.addlocalefile(e.id);
		quest::read(temp, false);
		auto pe = bsdata<quest>::end();
		e.encounters = quests(pb, pe - pb);
	}
}

static void initialization() {
	bsreq::read("rules/Worldmap.txt");
	bsreq::read("rules/Items.txt");
	bsreq::read("rules/Monsters.txt");
	bsreq::read("rules/Investigators.txt");
	read_quests();
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