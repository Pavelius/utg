#include "bsref.h"
#include "bsreq.h"
#include "charname.h"
#include "draw_object.h"
#include "draw_utg.h"
#include "main.h"

static creature* create_player(class_s type, gender_s gender, feat_s feat) {
	auto p = bsdata<creature>::add();
	p->create(type, gender);
	p->set(feat);
	return p;
}

static void starting() {
	bsref<itemi> test = bsdata<itemi>::elements + 2;
	auto p1 = create_player(Fighter, Male, Player);
	auto p2 = create_player(Fighter, Male, Player);
	answers::header = "Большой зал";
	game.encounter(bsdata<monsteri>::find("Skeleton"));
	creatures.select();
	//treasure loot;
	//loot.generate('A');
	//loot.take();
	combat_mode();
}

void initialize_ui();
void update_monsters_stats();

static void initializing() {
	bsreq::read("rules/Items.txt");
	bsreq::read("rules/Monsters.txt");
	charname::read("locale/ru/NameCharacters.txt");
	generatori::read("rules/RandomGems.txt");
	update_monsters_stats();
}

static const char* getavatarst(const void* p) {
	if(!(*((creature*)p)))
		return 0;
	return ((creature*)p)->avatarable::getavatar();
}

int main(int argc, char* argv[]) {
	draw::heroes = bsdata<creature>::source_ptr;
	draw::heroes_getavatar = getavatarst;
	//srand(getcputime());
	srand(1123);
	answers::console = &utg::sb;
	answers::prompt = utg::sb.begin();
	answers::resid = "meet";
	draw::object::initialize();
	initialize_ui();
	return draw::start(starting, initializing);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}