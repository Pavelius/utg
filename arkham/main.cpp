#include "bsreq.h"
#include "draw_object.h"
#include "main.h"
#include "strategy.h"

#ifdef _DEBUG

void util_main();

static void test_fight() {
	cardi card;
	card.clear();
	card.type = bsdata<cardprotoi>::source.find("Ghoul", 0);
	game.fight(card);
}

static void test_encounter() {
	game.create("CarolynFern");
	game.add(Sanity, -1);
	//game.introduction();
	//test_fight();
	//game.showcards();
	//game.refocus();
	cards.pick(Gate, 1);
	cards.pick(Monster, 6);
	//game.movement(bsdata<locationi>::find("SouthChurch"), false);
	game.play();
	//game.movement(game.get(Speed));
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
	char temp[260]; stringbuilder sb(temp);
	for(auto& e : bsdata<locationi>()) {
		if(e.type == Street)
			continue;
		auto pb = bsdata<quest>::end();
		sb.clear(); sb.addlocalefile(e.id);
		quest::read(temp, false);
		auto pe = bsdata<quest>::end();
		e.encounters = quests(pb, pe - pb);
	}
	auto pb = bsdata<quest>::end();
	sb.clear(); sb.addlocalefile("Other");
	quest::read(temp);
	auto pe = bsdata<quest>::end();
	game.quest_other = quests(pb, pe - pb);
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