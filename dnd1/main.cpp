#include "bsreq.h"
#include "charname.h"
#include "draw_object.h"
#include "main.h"

static creature* create_player(class_s type, gender_s gender, feat_s feat) {
	auto p = bsdata<creature>::add();
	p->create(type, gender);
	p->set(feat);
	return p;
}

static void starting() {
	auto p1 = create_player(Fighter, Male, Player);
	auto p2 = create_player(Fighter, Male, Enemy);
	game.creatures.select();
	//treasure loot;
	//loot.generate('A');
	//loot.take();
	combat_mode();
}

void initialize_ui();
void update_enchantments();
void update_monsters_stats();

static void initializing() {
	bsreq::read("rules/Items.txt");
	bsreq::read("rules/Monsters.txt");
	charname::read("locale/ru/NameCharacters.txt");
	generatori::read("rules/RandomGems.txt");
	update_enchantments();
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
	srand(getcputime());
	answers::console = &utg::sb;
	answers::prompt = utg::sb.begin();
	answers::resid = "meet";
	draw::object::initialize();
	initialize_ui();
	return draw::start(starting, true, initializing);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}