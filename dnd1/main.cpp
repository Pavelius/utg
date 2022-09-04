#include "bsreq.h"
#include "charname.h"
#include "draw_object.h"
#include "main.h"

static creature* create_player(class_s type, gender_s gender, feat_s feat) {
	auto p = bsdata<creature>::add();
	p->create(type, gender);
	p->set(feat);
	p->name = p->randomname(type, gender);
	return p;
}

static void starting() {
	auto p1 = create_player(Fighter, Male, Player);
	auto p2 = create_player(Fighter, Male, Enemy);
	game.creatures.select();
	treasure loot;
	loot.generate('A');
	loot.take();
	combat_mode();
}

static void initializing() {
	bsreq::read("rules/Items.txt");
	bsreq::read("rules/Monsters.txt");
	charname::read("locale/ru/NameCharacters.txt");
	generatori::read("rules/RandomGems.txt");
}

int main(int argc, char* argv[]) {
	//utg::callback::getinfo = game.sfgetproperty;
	//utg::callback::getstatus = game.sfgetstatus;
	srand(getcputime());
	answers::console = &utg::sb;
	answers::prompt = utg::sb.begin();
	draw::object::initialize();
	//goal::info = game.information;
	return draw::start(starting, true, initializing);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}